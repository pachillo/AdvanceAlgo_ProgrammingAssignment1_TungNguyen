#include <complex>
#include <vector>
#include <cmath>
#include <numbers>

using Complex = std::complex<double>;
using ComplexVector = std::vector<Complex>;

// Finds the smallest non-trivial factor of n (> 1). Returns n if n is prime.
std::size_t find_smallest_factor(std::size_t n) {
    if (n <= 3) return n;
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;
    for (std::size_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0) return i;
        if (n % (i + 2) == 0) return i + 2;
    }
    return n; // n is prime
}

// Direct O(n^2) DFT for prime base cases
ComplexVector naive_dft(const ComplexVector& input) {
    const std::size_t n = input.size();
    ComplexVector output(n, Complex(0.0, 0.0));
    const double angle_factor = -2.0 * std::numbers::pi / static_cast<double>(n);

    for (std::size_t k = 0; k < n; ++k) {
        for (std::size_t t = 0; t < n; ++t) {
            const double theta = angle_factor * static_cast<double>(k * t);
            output[k] += input[t] * std::polar(1.0, theta);
        }
    }
    return output;
}

ComplexVector general_cooley_tukey_fft(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1) {
        return input;
    }

    const std::size_t n1 = find_smallest_factor(n);

    // Base case: If n is prime, we cannot factor further with Cooley-Tukey
    if (n1 == n) {
        return naive_dft(input);
    }

    const std::size_t n2 = n / n1;

    // 1. Decompose input into n1 sub-vectors of size n2
    // sub_transforms[n1_idx] stores the recursive FFT of slice input[n1_idx + j * n1]
    std::vector<ComplexVector> sub_transforms(n1, ComplexVector(n2));
    for (std::size_t i = 0; i < n1; ++i) {
        ComplexVector slice(n2);
        for (std::size_t j = 0; j < n2; ++j) {
            slice[j] = input[i + j * n1];
        }
        sub_transforms[i] = general_cooley_tukey_fft(slice);
    }

    ComplexVector res(n);
    const double angle_factor = -2.0 * std::numbers::pi / static_cast<double>(n);

    // 2. Recombination: Twiddle multiplication + n1-point DFT per bin
    for (std::size_t k2 = 0; k2 < n2; ++k2) {
        for (std::size_t k1 = 0; k1 < n1; ++k1) {
            Complex sum(0.0, 0.0);

            for (std::size_t n1_idx = 0; n1_idx < n1; ++n1_idx) {
                // Twiddle factor: exp(-2*pi*i * (n1_idx * k2) / n)
                const double twiddle_angle = angle_factor * static_cast<double>(n1_idx * k2);
                const Complex twiddle = std::polar(1.0, twiddle_angle);

                // Internal n1-point DFT phase: exp(-2*pi*i * (n1_idx * k1) / n1)
                const double dft_angle = -2.0 * std::numbers::pi * static_cast<double>(n1_idx * k1) 
                                         / static_cast<double>(n1);
                const Complex dft_kernel = std::polar(1.0, dft_angle);

                sum += sub_transforms[n1_idx][k2] * twiddle * dft_kernel;
            }

            // Output mapping: index k = k1 * n2 + k2
            res[k1 * n2 + k2] = sum;
        }
    }

    return res;
}
