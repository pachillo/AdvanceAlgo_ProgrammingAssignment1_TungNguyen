#pragma once

#include "types.hpp"

namespace algorithm {

ComplexVector precompute_twiddles(std::size_t n);
ComplexVector precompute_twiddles_ifft(std::size_t n);
ComplexVector precompute_twiddles_for_ifft(std::size_t n);

ComplexVector precomputed_twiddle_fft(const ComplexVector& input);
ComplexVector precomputed_twiddle_fft(const ComplexVector& input, const ComplexVector& W);

ComplexVector precomputed_twiddle_ifft(const ComplexVector& input);
ComplexVector precomputed_twiddle_ifft(const ComplexVector& input, const ComplexVector& W);

}  // namespace algorithm
