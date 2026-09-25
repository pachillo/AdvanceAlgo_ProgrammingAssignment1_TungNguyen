#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>

#include "fft/cooley_tukey.hpp"
#include "fft/naive_dft.hpp"
#include "fft/types.hpp"

namespace {

constexpr double EPSILON = 1e-9;

void expect_true(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "Assertion failed: " << message << '\n';
        std::exit(1);
    }
}

bool approx_equal(const fft::Complex& a, const fft::Complex& b, double eps = EPSILON) {
    return std::abs(a - b) < eps;
}

bool vector_approx_equal(const fft::ComplexVector& a, const fft::ComplexVector& b,
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
    const fft::ComplexVector input = {{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}};

    const auto dft = fft::naive_dft(input);
    const auto fft_res = fft::cooley_tukey_fft(input);

    expect_true(vector_approx_equal(dft, fft_res), "DFT and FFT results do not match for N=4");
    std::cout << "PASSED\n";
}

void test_roundtrip_ifft() {
    std::cout << "[TEST] IFFT(FFT(x)) == x round-trip... ";
    const fft::ComplexVector input = {{1.5, -2.0}, {3.2, 0.5}, {-0.7, 4.1},  {2.2, -1.1},
                                      {0.0, 0.0},  {5.1, 1.2}, {-3.3, -2.4}, {1.0, 1.0}};

    const auto freq = fft::cooley_tukey_fft(input);
    const auto reconstructed = fft::cooley_tukey_ifft(freq);

    expect_true(vector_approx_equal(input, reconstructed), "Round-trip IFFT(FFT(x)) failed");
    std::cout << "PASSED\n";
}

void test_random_vectors_comparison() {
    std::cout << "[TEST] Random vectors up to N=512 comparison... ";
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> dist(-100.0, 100.0);

    const std::size_t sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
    for (std::size_t n : sizes) {
        fft::ComplexVector input(n);
        for (std::size_t i = 0; i < n; ++i) {
            input[i] = fft::Complex{dist(rng), dist(rng)};
        }

        const auto dft = fft::naive_dft(input);
        const auto fft_res = fft::cooley_tukey_fft(input);

        expect_true(vector_approx_equal(dft, fft_res, 1e-8), "DFT and FFT mismatch on random data");
    }
    std::cout << "PASSED\n";
}

void test_invalid_length_exception() {
    std::cout << "[TEST] Non-power-of-2 exception handling... ";
    const fft::ComplexVector invalid_input(3, {1.0, 0.0});
    bool caught = false;
    try {
        fft::cooley_tukey_fft(invalid_input);
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    expect_true(caught, "Did not throw std::invalid_argument for non-power-of-2 size");
    std::cout << "PASSED\n";
}

}  // namespace

int main() {
    std::cout << "Running FFT verification test suite...\n";
    test_basic_four_elements();
    test_roundtrip_ifft();
    test_random_vectors_comparison();
    test_invalid_length_exception();
    std::cout << "All tests passed successfully!\n";
    return 0;
}
