#pragma once

#include "fft/types.hpp"

namespace fft {

/**
 * @brief Checks if a given integer is a power of 2.
 */
constexpr bool is_power_of_two(std::size_t n) noexcept {
    return n > 0 && (n & (n - 1)) == 0;
}

/**
 * @brief Computes Radix-2 Decimation-In-Time (DIT) Cooley-Tukey Fast Fourier Transform (O(N log
 * N)).
 *
 * Note: Input length N must be a power of 2.
 *
 * @param input The input time-domain discrete complex vector of length N (power of 2).
 * @return ComplexVector The output frequency-domain complex vector of length N.
 */
ComplexVector cooley_tukey_fft(const ComplexVector& input);

/**
 * @brief Computes Inverse Fast Fourier Transform using Cooley-Tukey algorithm.
 *
 * @param input The input frequency-domain complex vector of length N (power of 2).
 * @return ComplexVector The reconstructed time-domain complex vector of length N.
 */
ComplexVector cooley_tukey_ifft(const ComplexVector& input);

}  // namespace fft
