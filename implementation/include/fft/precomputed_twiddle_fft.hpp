#pragma once

#include "fft/types.hpp"

namespace fft {

/**
 * @brief Computes Discrete Fourier Transform (DFT) using the O(N^2) naive summation.
 *
 * Formula:
 *   X[k] = \sum_{j=0}^{N-1} x[j] * exp(-2 * \pi * i * k * j / N)
 *
 * @param input The input time-domain discrete complex vector of length N.
 * @return ComplexVector The output frequency-domain complex vector of length N.
 */
ComplexVector precomputed_twiddle_fft(const ComplexVector& input);

/**
 * @brief Computes Inverse Discrete Fourier Transform (IDFT) using naive summation.
 *
 * @param input The input frequency-domain complex vector of length N.
 * @return ComplexVector The reconstructed time-domain complex vector of length N.
 */
ComplexVector precomputed_twiddle_ifft(const ComplexVector& input);

}  // namespace fft
