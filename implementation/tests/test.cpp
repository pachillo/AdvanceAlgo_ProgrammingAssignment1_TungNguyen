#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>

#include "iterative_radix-2_fft.hpp"
#include "mixed-radix_fft.hpp"
#include "naive_dft.hpp"
#include "precomputed_twiddle_fft.hpp"
#include "recursive_radix-2_fft.hpp"
#include "types.hpp"

namespace {

constexpr double EPSILON = 1e-9;

void expect_true(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "Assertion failed: " << message << '\n';
        std::exit(1);
    }
}

bool approx_equal(const algorithm::Complex& a, const algorithm::Complex& b, double eps = EPSILON) {
    return std::abs(a - b) < eps;
}

bool vector_approx_equal(const algorithm::ComplexVector& a, const algorithm::ComplexVector& b,
                         double eps = EPSILON) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (!approx_equal(a[i], b[i], eps)) {
            return false;
        }
    }
    return true;
}

void test_basic_four_elements() {
    std::cout << "[TEST] 4-element basic comparison... ";
    const algorithm::ComplexVector input = {{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}};

    const auto dft = algorithm::naive_dft(input);
    const auto fft_res = algorithm::recursive_radix2_fft(input);

    expect_true(vector_approx_equal(dft, fft_res), "DFT and FFT results do not match for N=4");
    std::cout << "PASSED\n";
}

void test_roundtrip_ifft() {
    std::cout << "[TEST] IFFT(FFT(x)) == x round-trip... ";
    const algorithm::ComplexVector input = {{1.5, -2.0}, {3.2, 0.5}, {-0.7, 4.1},  {2.2, -1.1},
                                            {0.0, 0.0},  {5.1, 1.2}, {-3.3, -2.4}, {1.0, 1.0}};

    const auto freq = algorithm::recursive_radix2_fft(input);
    const auto reconstructed = algorithm::recursive_radix2_ifft(freq);

    expect_true(vector_approx_equal(input, reconstructed), "Round-trip IFFT(FFT(x)) failed");
    std::cout << "PASSED\n";
}

void test_random_vectors_comparison() {
    std::cout << "[TEST] Random vectors up to N=512 comparison... ";
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> dist(-100.0, 100.0);

    const std::size_t sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
    for (std::size_t n : sizes) {
        algorithm::ComplexVector input(n);
        for (std::size_t i = 0; i < n; ++i) {
            input[i] = algorithm::Complex{dist(rng), dist(rng)};
        }

        const auto dft = algorithm::naive_dft(input);
        const auto fft_res = algorithm::recursive_radix2_fft(input);

        expect_true(vector_approx_equal(dft, fft_res, 1e-8), "DFT and FFT mismatch on random data");
    }
    std::cout << "PASSED\n";
}

void test_invalid_length_exception() {
    std::cout << "[TEST] Non-power-of-2 exception handling... ";
    const algorithm::ComplexVector invalid_input(3, {1.0, 0.0});
    bool caught = false;
    try {
        algorithm::recursive_radix2_fft(invalid_input);
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    expect_true(caught, "Did not throw std::invalid_argument for non-power-of-2 size");
    std::cout << "PASSED\n";
}

void test_iterative_vs_recursive() {
    std::cout << "[TEST] Iterative vs Recursive FFT up to N=2048... ";
    std::mt19937_64 rng(123);
    std::uniform_real_distribution<double> dist(-50.0, 50.0);

    const std::size_t sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    for (std::size_t n : sizes) {
        algorithm::ComplexVector input(n);
        for (std::size_t i = 0; i < n; ++i) {
            input[i] = algorithm::Complex{dist(rng), dist(rng)};
        }

        const auto rec_res = algorithm::recursive_radix2_fft(input);
        const auto iter_res = algorithm::iterative_radix2_fft(input);

        expect_true(vector_approx_equal(rec_res, iter_res, 1e-8),
                    "Recursive and Iterative FFT mismatch");
    }
    std::cout << "PASSED\n";
}

void test_precomputed_vs_iterative() {
    std::cout << "[TEST] Precomputed vs Iterative FFT up to N=2048... ";
    std::mt19937_64 rng(456);
    std::uniform_real_distribution<double> dist(-50.0, 50.0);

    const std::size_t sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    for (std::size_t n : sizes) {
        algorithm::ComplexVector input(n);
        for (std::size_t i = 0; i < n; ++i) {
            input[i] = algorithm::Complex{dist(rng), dist(rng)};
        }

        const auto iter_res = algorithm::iterative_radix2_fft(input);
        const auto pre_res = algorithm::precomputed_twiddle_fft(input);

        expect_true(vector_approx_equal(iter_res, pre_res, 1e-8),
                    "Iterative and Precomputed FFT mismatch");
    }
    std::cout << "PASSED\n";
}

void test_mixed_radix() {
    std::cout << "[TEST] Mixed-Radix FFT vs Naive DFT & Roundtrip... ";
    std::mt19937_64 rng(789);
    std::uniform_real_distribution<double> dist(-50.0, 50.0);

    // Test primes, composite non-powers-of-2, and powers-of-2
    const std::size_t sizes[] = {2, 3, 5, 6, 7, 8, 9, 10, 12, 15, 16, 20, 24, 30, 48, 60, 64};
    for (std::size_t n : sizes) {
        algorithm::ComplexVector input(n);
        for (std::size_t i = 0; i < n; ++i) {
            input[i] = algorithm::Complex{dist(rng), dist(rng)};
        }

        const auto dft_res = algorithm::naive_dft(input);
        const auto mr_res = algorithm::mixed_radix_fft(input);
        expect_true(vector_approx_equal(dft_res, mr_res, 1e-8),
                    "Mixed-Radix FFT mismatch against Naive DFT");

        const auto reconstructed = algorithm::mixed_radix_ifft(mr_res);
        expect_true(vector_approx_equal(input, reconstructed, 1e-8),
                    "Mixed-Radix IFFT round-trip failed");
    }
    std::cout << "PASSED\n";
}

void test_edge_cases() {
    std::cout << "[TEST] Edge cases (N=0, N=1)... ";
    const algorithm::ComplexVector empty_vec{};
    const algorithm::ComplexVector single_vec{{42.0, -13.0}};

    // N = 0
    expect_true(algorithm::naive_dft(empty_vec).empty(), "Naive DFT N=0 failed");
    expect_true(algorithm::naive_idft(empty_vec).empty(), "Naive IDFT N=0 failed");
    expect_true(algorithm::recursive_radix2_fft(empty_vec).empty(), "Recursive FFT N=0 failed");
    expect_true(algorithm::recursive_radix2_ifft(empty_vec).empty(), "Recursive IFFT N=0 failed");
    expect_true(algorithm::iterative_radix2_fft(empty_vec).empty(), "Iterative FFT N=0 failed");
    expect_true(algorithm::iterative_radix2_ifft(empty_vec).empty(), "Iterative IFFT N=0 failed");
    expect_true(algorithm::bit_reverse_permutation(empty_vec).empty(), "Bit reversal N=0 failed");
    expect_true(algorithm::precomputed_twiddle_fft(empty_vec).empty(),
                "Precomputed FFT N=0 failed");
    expect_true(algorithm::precomputed_twiddle_ifft(empty_vec).empty(),
                "Precomputed IFFT N=0 failed");
    expect_true(algorithm::mixed_radix_fft(empty_vec).empty(), "Mixed-Radix FFT N=0 failed");
    expect_true(algorithm::mixed_radix_ifft(empty_vec).empty(), "Mixed-Radix IFFT N=0 failed");

    // N = 1
    expect_true(vector_approx_equal(algorithm::naive_dft(single_vec), single_vec),
                "Naive DFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::naive_idft(single_vec), single_vec),
                "Naive IDFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::recursive_radix2_fft(single_vec), single_vec),
                "Recursive FFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::recursive_radix2_ifft(single_vec), single_vec),
                "Recursive IFFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::iterative_radix2_fft(single_vec), single_vec),
                "Iterative FFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::iterative_radix2_ifft(single_vec), single_vec),
                "Iterative IFFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::bit_reverse_permutation(single_vec), single_vec),
                "Bit reversal N=1 failed");
    expect_true(vector_approx_equal(algorithm::precomputed_twiddle_fft(single_vec), single_vec),
                "Precomputed FFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::precomputed_twiddle_ifft(single_vec), single_vec),
                "Precomputed IFFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::mixed_radix_fft(single_vec), single_vec),
                "Mixed-Radix FFT N=1 failed");
    expect_true(vector_approx_equal(algorithm::mixed_radix_ifft(single_vec), single_vec),
                "Mixed-Radix IFFT N=1 failed");

    std::cout << "PASSED\n";
}

}  // namespace

int main() {
    std::cout << "Running FFT verification test suite...\n";
    test_edge_cases();
    test_basic_four_elements();
    test_roundtrip_ifft();
    test_random_vectors_comparison();
    test_invalid_length_exception();
    test_iterative_vs_recursive();
    test_precomputed_vs_iterative();
    test_mixed_radix();
    std::cout << "All tests passed successfully!\n";
    return 0;
}
