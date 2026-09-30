#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "mixed-radix_fft.hpp"
#include "naive_dft.hpp"
#include "types.hpp"

namespace {

using Clock = std::chrono::high_resolution_clock;

template <typename T>
inline void do_not_optimize(T&& val) {
    asm volatile("" : : "g"(val) : "memory");
}

void warmup_cpu(std::chrono::milliseconds duration = std::chrono::milliseconds(200)) {
    const auto start = Clock::now();
    volatile double dummy = 0.0;
    while (Clock::now() - start < duration) {
        dummy += 1.0;
        do_not_optimize(dummy);
    }
}

std::size_t get_iterations(std::size_t n) {
    if (n <= 16)
        return 20000;
    if (n <= 64)
        return 5000;
    if (n <= 256)
        return 1000;
    if (n <= 512)
        return 250;
    if (n <= 1024)
        return 80;
    if (n <= 2048)
        return 25;
    if (n <= 4096)
        return 10;
    return 5;  // n > 4096 (Naive takes ~0.1-0.2s each)
}

struct TestPoint {
    std::size_t n;
    std::string category;  // "Prime", "PowerOfTwo", "CompositeNonPowerOfTwo"
};

}  // namespace

int main(int argc, char* argv[]) {
    const std::string output_path = (argc > 1) ? argv[1] : "bench/results/experiment6.csv";

    std::cout << "========================================================================\n"
              << " Experiment 6: Prime vs. Composite Scaling in Mixed-Radix FFT\n"
              << " Comparing: Mixed-Radix FFT vs. Naive DFT on Primes & Composite inputs\n"
              << " Output CSV: " << output_path << "\n"
              << "========================================================================\n\n";

    std::ofstream csv(output_path);
    if (!csv.is_open()) {
        std::cerr << "Error: Could not open output file: " << output_path << '\n';
        return 1;
    }

    csv << "N,Category,Smallest_Factor,Iterations,Naive_Time_us,Mixed_Time_us,Speedup_Ratio\n";

    // 1. Prime sizes (cannot factorize, forces fallback to Naive DFT)
    const std::vector<std::size_t> primes = {7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191};

    // 2. Power-of-2 composite sizes (ideal factor tree of 2s)
    const std::vector<std::size_t> powers_of_two = {8,   16,   32,   64,   128, 256,
                                                    512, 1024, 2048, 4096, 8192};

    // 3. Smooth composite non-powers-of-2 (factors of 2, 3, 5)
    const std::vector<std::size_t> smooth_composites = {12,  24,  60,   120,  240,
                                                        480, 960, 1920, 3840, 7680};

    std::vector<TestPoint> test_points;
    for (std::size_t p : primes) {
        test_points.push_back({p, "Prime"});
    }
    for (std::size_t p2 : powers_of_two) {
        test_points.push_back({p2, "PowerOfTwo"});
    }
    for (std::size_t sc : smooth_composites) {
        test_points.push_back({sc, "CompositeNonPowerOfTwo"});
    }

    // Sort by N for clean chronological output
    std::sort(test_points.begin(), test_points.end(), [](const TestPoint& a, const TestPoint& b) {
        if (a.n != b.n)
            return a.n < b.n;
        return a.category < b.category;
    });

    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    warmup_cpu();

    std::cout << std::left << std::setw(8) << "N" << std::setw(26) << "Category" << std::setw(12)
              << "Factor" << std::setw(16) << "Naive (us)" << std::setw(16) << "Mixed-Radix (us)"
              << std::setw(12) << "Speedup" << '\n';
    std::cout << std::string(90, '-') << '\n';

    for (const auto& tp : test_points) {
        const std::size_t n = tp.n;
        const std::size_t factor = algorithm::find_smallest_factor(n);
        const std::size_t iters = get_iterations(n);

        // Generate deterministic input vector
        algorithm::ComplexVector input(n);
        for (std::size_t i = 0; i < n; ++i) {
            input[i] = algorithm::Complex{dist(rng), dist(rng)};
        }

        // --- Benchmark Naive DFT ---
        // Warmup
        auto res_naive = algorithm::naive_dft(input);
        do_not_optimize(res_naive.data());

        const auto start_naive = Clock::now();
        for (std::size_t i = 0; i < iters; ++i) {
            auto out = algorithm::naive_dft(input);
            do_not_optimize(out.data());
        }
        const auto end_naive = Clock::now();
        const double naive_us =
            std::chrono::duration<double, std::micro>(end_naive - start_naive).count() /
            static_cast<double>(iters);

        // --- Benchmark Mixed-Radix FFT ---
        // Warmup
        auto res_mixed = algorithm::mixed_radix_fft(input);
        do_not_optimize(res_mixed.data());

        const auto start_mixed = Clock::now();
        for (std::size_t i = 0; i < iters; ++i) {
            auto out = algorithm::mixed_radix_fft(input);
            do_not_optimize(out.data());
        }
        const auto end_mixed = Clock::now();
        const double mixed_us =
            std::chrono::duration<double, std::micro>(end_mixed - start_mixed).count() /
            static_cast<double>(iters);

        const double speedup = naive_us / mixed_us;

        // Print to console
        std::cout << std::left << std::setw(8) << n << std::setw(26) << tp.category << std::setw(12)
                  << factor << std::fixed << std::setprecision(2) << std::setw(16) << naive_us
                  << std::setw(16) << mixed_us << std::setprecision(2) << speedup << "x" << '\n';

        // Write to CSV
        csv << n << ',' << tp.category << ',' << factor << ',' << iters << ',' << std::fixed
            << std::setprecision(4) << naive_us << ',' << mixed_us << ',' << std::setprecision(3)
            << speedup << '\n';
        csv.flush();
    }

    std::cout << "\nBenchmark complete. Results saved to: " << output_path << '\n';
    return 0;
}
