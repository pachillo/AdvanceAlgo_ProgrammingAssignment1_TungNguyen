#pragma once

#include "fft/types.hpp"

namespace fft {

constexpr bool is_power_of_two(std::size_t n) noexcept {
    return n > 0 && (n & (n - 1)) == 0;
}

/**
 * @brief Computes Discrete Fourier Transform (DFT) using the O(N^2) naive summation.
 *
 * Formula:
 *   X[k] = \sum_{j=0}^{N-1} x[j] * exp(-2 * \pi * i * k * j / N)
 *
 * @param input The input time-domain discrete complex vector of length N.
 * @return ComplexVector The output frequency-domain complex vector of length N.
 */
ComplexVector 2radix_cooley_tukey(const ComplexVector& input);

/**
 * @brief Computes Inverse Discrete Fourier Transform (IDFT) using naive summation.
 *
 * @param input The input frequency-domain complex vector of length N.
 * @return ComplexVector The reconstructed time-domain complex vector of length N.
 */
ComplexVector i2radix_cooley_tukey(const ComplexVector& input);

}  // namespace fft
