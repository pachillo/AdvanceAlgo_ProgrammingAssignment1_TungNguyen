#include <algorithm>
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

// Warm up the CPU to transition out of idle/power-saving state
void warmup_cpu() {
    volatile double dummy = 1.0001;
    const auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now() - start)
               .count() < 300) {
        for (int i = 0; i < 50000; ++i) {
            dummy = dummy * 1.000001 + 0.000001;
        }
    }
    do_not_optimize(dummy);
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

double run_iterative_batch(const algorithm::ComplexVector& signal, std::size_t iterations) {
    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto res = algorithm::iterative_radix2_fft(signal);
        do_not_optimize(res);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return static_cast<double>(total_ns) / static_cast<double>(iterations);
}

double measure_iterative_fft_ns(const algorithm::ComplexVector& signal, std::size_t iterations) {
    // Warmup
    for (std::size_t i = 0; i < std::min<std::size_t>(iterations, 5); ++i) {
        auto res = algorithm::iterative_radix2_fft(signal);
        do_not_optimize(res);
    }

    // Minimum of 3 runs to reject OS scheduling noise and thermal jitter
    double best = run_iterative_batch(signal, iterations);
    for (int r = 0; r < 2; ++r) {
        best = std::min(best, run_iterative_batch(signal, iterations));
    }
    return best;
}

double run_precomputed_batch(const algorithm::ComplexVector& signal,
                             const algorithm::ComplexVector& W, std::size_t iterations) {
    const auto start = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        auto res = algorithm::precomputed_twiddle_fft(signal, W);
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

    // Minimum of 3 runs
    double best = run_precomputed_batch(signal, W, iterations);
    for (int r = 0; r < 2; ++r) {
        best = std::min(best, run_precomputed_batch(signal, W, iterations));
    }
    return best;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string output_path = "bench/results/experiment4.csv";
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

    // Wake up CPU to avoid power-saving governor scaling artifacts
    warmup_cpu();

    csv << "N,Working_Set_KB,Iterative_mean_ns,Precomputed_mean_ns,Tau_Iterative_ns,Tau_"
           "Precomputed_ns\n";

    std::cout << "================================================================================="
                 "=======\n";
    std::cout << " Experiment 4: The Hardware Cache Cliff Effect (N = 2^1 to 2^20)                 "
                 "      \n";
    std::cout << " Normalized Execution Time: tau(N) = Time(N) / (N * log2(N))                     "
                 "      \n";
    std::cout << "================================================================================="
                 "=======\n";
    std::cout << std::setw(10) << "N" << std::setw(12) << "Size (KB)" << std::setw(18)
              << "Iter Time (ns)" << std::setw(18) << "Pre Time (ns)" << std::setw(18)
              << "tau_iter (ns)" << std::setw(18) << "tau_pre (ns)" << '\n';
    std::cout << "---------------------------------------------------------------------------------"
                 "-------\n";

    for (std::size_t power = 1; power <= 20; ++power) {
        const std::size_t n = static_cast<std::size_t>(1) << power;
        const std::size_t iters = determine_iterations(n);
        const auto signal = generate_random_signal(n);
        const auto W = algorithm::precompute_twiddles(n);

        const double iter_ns = measure_iterative_fft_ns(signal, iters);
        const double pre_ns = measure_precomputed_fft_ns(signal, W, iters);

        const double log2_n = static_cast<double>(power);
        const double n_log2_n = static_cast<double>(n) * log2_n;
        const double tau_iter = iter_ns / n_log2_n;
        const double tau_pre = pre_ns / n_log2_n;
        const double working_set_kb = static_cast<double>(n * 16) / 1024.0;

        csv << n << ',' << std::fixed << std::setprecision(2) << working_set_kb << ',' << std::fixed
            << std::setprecision(2) << iter_ns << ',' << std::fixed << std::setprecision(2)
            << pre_ns << ',' << std::fixed << std::setprecision(4) << tau_iter << ',' << std::fixed
            << std::setprecision(4) << tau_pre << '\n';

        std::cout << std::setw(10) << n << std::setw(11) << std::fixed << std::setprecision(1)
                  << working_set_kb << "K" << std::setw(18) << std::fixed << std::setprecision(1)
                  << iter_ns << std::setw(18) << std::fixed << std::setprecision(1) << pre_ns
                  << std::setw(18) << std::fixed << std::setprecision(2) << tau_iter
                  << std::setw(18) << std::fixed << std::setprecision(2) << tau_pre << '\n';
    }

    csv.close();
    std::cout << "---------------------------------------------------------------------------------"
                 "-------\n";
    std::cout << "Benchmark results written successfully to: " << output_path << "\n\n";

    return 0;
}
