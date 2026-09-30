#pragma once

#include "types.hpp"

namespace algorithm {
ComplexVector bit_reverse_permutation(const ComplexVector& input);
ComplexVector iterative_radix2_fft(const ComplexVector& input);
ComplexVector iterative_radix2_ifft(const ComplexVector& input);
}  // namespace algorithm
