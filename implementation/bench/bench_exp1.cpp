#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "naive_dft.hpp"
#include "recursive_radix-2_fft.hpp"
#include "types.hpp"

namespace fs = std::filesystem;

namespace {

// Compiler memory barrier to prevent dead code elimination of computation
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
        return 1000;
    }
    if (n <= 1024) {
        return 100;
    }
    if (n <= 4096) {
        return 20;
    }
    return 5;  // n = 8192
}

double measure_dft_ns(const algorithm::ComplexVector& signal, std::size_t iterations) {
    // Warmup
    for (std::size_t i = 0; i < std::min<std::size_t>(iterations, 5); ++i) {
        auto res = algorithm::naive_dft(signal);
        do_not_optimize(res);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto res = algorithm::naive_dft(signal);
        do_not_optimize(res);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return static_cast<double>(total_ns) / static_cast<double>(iterations);
}

double measure_fft_ns(const algorithm::ComplexVector& signal, std::size_t iterations) {
    // Warmup
    for (std::size_t i = 0; i < std::min<std::size_t>(iterations, 5); ++i) {
        auto res = algorithm::recursive_radix2_fft(signal);
        do_not_optimize(res);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto res = algorithm::recursive_radix2_fft(signal);
        do_not_optimize(res);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return static_cast<double>(total_ns) / static_cast<double>(iterations);
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string output_path = "bench/results/experiment1.csv";
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

    csv << "N,DFT_mean_ns,FFT_mean_ns,Ratio_DFT_over_FFT\n";

    std::cout << "=========================================================================\n";
    std::cout << " Experiment 1: Asymptotic Scaling & Crossover Point (N = 2^1 to 2^13)     \n";
    std::cout << "=========================================================================\n";
    std::cout << std::setw(8) << "N" << std::setw(15) << "Iterations" << std::setw(18)
              << "DFT Time (ns)" << std::setw(18) << "FFT Time (ns)" << std::setw(15)
              << "Speedup (x)" << '\n';
    std::cout << "-------------------------------------------------------------------------\n";

    for (std::size_t power = 1; power <= 13; ++power) {
        const std::size_t n = static_cast<std::size_t>(1) << power;
        const std::size_t iters = determine_iterations(n);
        const auto signal = generate_random_signal(n);

        const double dft_ns = measure_dft_ns(signal, iters);
        const double fft_ns = measure_fft_ns(signal, iters);
        const double ratio = dft_ns / fft_ns;

        csv << n << ',' << std::fixed << std::setprecision(2) << dft_ns << ',' << std::fixed
            << std::setprecision(2) << fft_ns << ',' << std::fixed << std::setprecision(4) << ratio
            << '\n';

        std::cout << std::setw(8) << n << std::setw(15) << iters << std::setw(18) << std::fixed
                  << std::setprecision(1) << dft_ns << std::setw(18) << std::fixed
                  << std::setprecision(1) << fft_ns << std::setw(15) << std::fixed
                  << std::setprecision(2) << ratio
                  << (ratio >= 1.0 ? " (FFT faster)" : " (DFT faster)") << '\n';
    }

    csv.close();
    std::cout << "-------------------------------------------------------------------------\n";
    std::cout << "Benchmark results written successfully to: " << output_path << "\n\n";

    return 0;
}
