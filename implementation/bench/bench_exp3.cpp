#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "iterative_radix-2_fft.hpp"
#include "precomputed_twiddle_fft.hpp"
#include "types.hpp"

namespace fs = std::filesystem;

namespace {

// Compiler memory barrier to prevent dead code elimination
template <typename T>
inline void do_not_optimize(T&& val) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "g"(val) : "memory");
#else
    volatile auto sink = &val;
    (void)sink;
#endif
}

algorithm::ComplexVector generate_random_signal(std::size_t n, uint64_t seed = 42) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    algorithm::ComplexVector signal(n);
    for (std::size_t i = 0; i < n; ++i) {
        signal[i] = algorithm::Complex{dist(rng), dist(rng)};
    }
    return signal;
}

std::size_t determine_iterations(std::size_t n) {
    if (n <= 16) {
        return 50000;
    }
    if (n <= 64) {
        return 10000;
    }
    if (n <= 256) {
        return 2000;
    }
    if (n <= 1024) {
        return 500;
    }
    if (n <= 4096) {
        return 100;
    }
    if (n <= 16384) {
        return 40;
    }
    if (n <= 65536) {
        return 15;
    }
    if (n <= 262144) {
        return 8;
    }
    return 4;  // n = 524288, 1048576
}

double measure_iterative_fft_ns(const algorithm::ComplexVector& signal, std::size_t iterations) {
    // Warmup
    for (std::size_t i = 0; i < std::min<std::size_t>(iterations, 5); ++i) {
        auto res = algorithm::iterative_radix2_fft(signal);
        do_not_optimize(res);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto res = algorithm::iterative_radix2_fft(signal);
        do_not_optimize(res);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return static_cast<double>(total_ns) / static_cast<double>(iterations);
}

double measure_precomputed_fft_ns(const algorithm::ComplexVector& signal,
                                  const algorithm::ComplexVector& W, std::size_t iterations) {
    // Warmup
    for (std::size_t i = 0; i < std::min<std::size_t>(iterations, 5); ++i) {
        auto res = algorithm::precomputed_twiddle_fft(signal, W);
        do_not_optimize(res);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto res = algorithm::precomputed_twiddle_fft(signal, W);
        do_not_optimize(res);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return static_cast<double>(total_ns) / static_cast<double>(iterations);
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string output_path = "bench/results/experiment3.csv";
    if (argc > 1) {
        output_path = argv[1];
    }

    fs::path out_file(output_path);
    if (out_file.has_parent_path()) {
        fs::create_directories(out_file.parent_path());
    }

    std::ofstream csv(output_path);
    if (!csv.is_open()) {
        std::cerr << "Failed to open output CSV file: " << output_path << '\n';
        return 1;
    }

    csv << "N,Iterative_mean_ns,Precomputed_mean_ns,Speedup_Factor\n";

    std::cout << "=========================================================================\n";
    std::cout << " Experiment 3: Trigonometric Math vs Cache Lookup (N = 2^1 to 2^20)       \n";
    std::cout << " Iterative (std::polar on the fly) vs. Precomputed Twiddles (Table Lookup)\n";
    std::cout << "=========================================================================\n";
    std::cout << std::setw(10) << "N" << std::setw(12) << "Iterations" << std::setw(20)
              << "Iterative (ns)" << std::setw(20) << "Precomputed (ns)" << std::setw(15)
              << "Speedup (x)" << '\n';
    std::cout << "-------------------------------------------------------------------------\n";

    for (std::size_t power = 1; power <= 20; ++power) {
        const std::size_t n = static_cast<std::size_t>(1) << power;
        const std::size_t iters = determine_iterations(n);
        const auto signal = generate_random_signal(n);

        // Precompute twiddle table beforehand (testing cache lookup during execution)
        const auto W = algorithm::precompute_twiddles(n);

        const double iter_ns = measure_iterative_fft_ns(signal, iters);
        const double pre_ns = measure_precomputed_fft_ns(signal, W, iters);
        const double speedup = iter_ns / pre_ns;

        csv << n << ',' << std::fixed << std::setprecision(2) << iter_ns << ',' << std::fixed
            << std::setprecision(2) << pre_ns << ',' << std::fixed << std::setprecision(4)
            << speedup << '\n';

        std::cout << std::setw(10) << n << std::setw(12) << iters << std::setw(20) << std::fixed
                  << std::setprecision(1) << iter_ns << std::setw(20) << std::fixed
                  << std::setprecision(1) << pre_ns << std::setw(15) << std::fixed
                  << std::setprecision(2) << speedup << "x" << '\n';
    }

    csv.close();
    std::cout << "-------------------------------------------------------------------------\n";
    std::cout << "Benchmark results written successfully to: " << output_path << "\n\n";

    return 0;
}
