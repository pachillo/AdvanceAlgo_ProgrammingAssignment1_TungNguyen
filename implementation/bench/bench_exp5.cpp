#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "iterative_radix-2_fft.hpp"
#include "naive_dft.hpp"
#include "precomputed_twiddle_fft.hpp"
#include "recursive_radix-2_fft.hpp"
#include "types.hpp"

namespace {

struct ErrorMetrics {
    double max_linf{0.0};
    double mean_linf{0.0};
    double mean_rmse{0.0};
};

ErrorMetrics compute_reconstruction_error(
    const std::vector<algorithm::ComplexVector>& inputs,
    const std::function<algorithm::ComplexVector(const algorithm::ComplexVector&)>& forward,
    const std::function<algorithm::ComplexVector(const algorithm::ComplexVector&)>& inverse) {
    if (inputs.empty()) {
        return {};
    }

    double max_linf = 0.0;
    double sum_linf = 0.0;
    double sum_rmse = 0.0;

    for (const auto& original : inputs) {
        const std::size_t n = original.size();
        const auto freq = forward(original);
        const auto reconstructed = inverse(freq);

        double trial_linf = 0.0;
        double trial_sum_sq = 0.0;

        for (std::size_t i = 0; i < n; ++i) {
            const double diff = std::abs(original[i] - reconstructed[i]);
            if (diff > trial_linf) {
                trial_linf = diff;
            }
            trial_sum_sq += diff * diff;
        }

        const double trial_rmse = std::sqrt(trial_sum_sq / static_cast<double>(n));

        if (trial_linf > max_linf) {
            max_linf = trial_linf;
        }
        sum_linf += trial_linf;
        sum_rmse += trial_rmse;
    }

    const double k = static_cast<double>(inputs.size());
    return {max_linf, sum_linf / k, sum_rmse / k};
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string output_path = (argc > 1) ? argv[1] : "bench/results/experiment5.csv";

    std::cout << "===============================================================\n"
              << " Experiment 5: Numerical Precision Drift (Round-trip Error)\n"
              << " Comparing: Naive DFT, Recursive FFT, Iterative FFT, Precomputed\n"
              << " Output CSV: " << output_path << "\n"
              << "===============================================================\n\n";

    std::ofstream csv(output_path);
    if (!csv.is_open()) {
        std::cerr << "Error: Could not open output file: " << output_path << '\n';
        return 1;
    }

    csv << "N,Trials,"
        << "Naive_max_Linf,Naive_mean_Linf,Naive_mean_RMSE,"
        << "Recursive_max_Linf,Recursive_mean_Linf,Recursive_mean_RMSE,"
        << "Iterative_max_Linf,Iterative_mean_Linf,Iterative_mean_RMSE,"
        << "Precomputed_max_Linf,Precomputed_mean_Linf,Precomputed_mean_RMSE\n";

    constexpr std::size_t MIN_POW = 1;         // N = 2^1 = 2
    constexpr std::size_t MAX_POW = 20;        // N = 2^20 = 1,048,576
    constexpr std::size_t NAIVE_MAX_POW = 13;  // N = 8192 (O(N^2) cutoff)
    constexpr std::size_t NUM_TRIALS = 5;

    std::mt19937_64 rng(1337);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    std::cout << std::left << std::setw(10) << "N" << std::setw(18) << "Naive L_inf"
              << std::setw(18) << "Recursive L_inf" << std::setw(18) << "Iterative L_inf"
              << std::setw(18) << "Precomp L_inf" << '\n';
    std::cout << std::string(82, '-') << '\n';

    for (std::size_t p = MIN_POW; p <= MAX_POW; ++p) {
        const std::size_t n = static_cast<std::size_t>(1) << p;

        // Generate independent pseudo-random input vectors
        std::vector<algorithm::ComplexVector> trial_inputs(NUM_TRIALS, algorithm::ComplexVector(n));
        for (std::size_t t = 0; t < NUM_TRIALS; ++t) {
            for (std::size_t i = 0; i < n; ++i) {
                trial_inputs[t][i] = algorithm::Complex{dist(rng), dist(rng)};
            }
        }

        // 1. Naive DFT (up to NAIVE_MAX_POW)
        ErrorMetrics naive_err{};
        bool has_naive = (p <= NAIVE_MAX_POW);
        if (has_naive) {
            naive_err = compute_reconstruction_error(
                trial_inputs,
                [](const algorithm::ComplexVector& x) { return algorithm::naive_dft(x); },
                [](const algorithm::ComplexVector& X) { return algorithm::naive_idft(X); });
        }

        // 2. Recursive Radix-2 FFT
        const auto rec_err = compute_reconstruction_error(
            trial_inputs,
            [](const algorithm::ComplexVector& x) { return algorithm::recursive_radix2_fft(x); },
            [](const algorithm::ComplexVector& X) { return algorithm::recursive_radix2_ifft(X); });

        // 3. Iterative Radix-2 FFT
        const auto iter_err = compute_reconstruction_error(
            trial_inputs,
            [](const algorithm::ComplexVector& x) { return algorithm::iterative_radix2_fft(x); },
            [](const algorithm::ComplexVector& X) { return algorithm::iterative_radix2_ifft(X); });

        // 4. Precomputed Twiddles Radix-2 FFT
        const auto W_fwd = algorithm::precompute_twiddles(n);
        const auto W_inv = algorithm::precompute_twiddles_for_ifft(n);
        const auto pre_err = compute_reconstruction_error(
            trial_inputs,
            [&W_fwd](const algorithm::ComplexVector& x) {
                return algorithm::precomputed_twiddle_fft(x, W_fwd);
            },
            [&W_inv](const algorithm::ComplexVector& X) {
                return algorithm::precomputed_twiddle_ifft(X, W_inv);
            });

        // Print to console
        std::cout << std::left << std::setw(10) << n;
        if (has_naive) {
            std::cout << std::scientific << std::setprecision(3) << std::setw(18)
                      << naive_err.max_linf;
        } else {
            std::cout << std::setw(18) << "N/A (O(N^2))";
        }
        std::cout << std::scientific << std::setprecision(3) << std::setw(18) << rec_err.max_linf
                  << std::setw(18) << iter_err.max_linf << std::setw(18) << pre_err.max_linf
                  << '\n';

        // Write to CSV
        csv << n << ',' << NUM_TRIALS << ',';
        if (has_naive) {
            csv << std::scientific << std::setprecision(8) << naive_err.max_linf << ','
                << naive_err.mean_linf << ',' << naive_err.mean_rmse << ',';
        } else {
            csv << ",,,";
        }
        csv << std::scientific << std::setprecision(8) << rec_err.max_linf << ','
            << rec_err.mean_linf << ',' << rec_err.mean_rmse << ',' << iter_err.max_linf << ','
            << iter_err.mean_linf << ',' << iter_err.mean_rmse << ',' << pre_err.max_linf << ','
            << pre_err.mean_linf << ',' << pre_err.mean_rmse << '\n';
        csv.flush();
    }

    std::cout << "\nBenchmark complete. Results saved to: " << output_path << '\n';
    return 0;
}
