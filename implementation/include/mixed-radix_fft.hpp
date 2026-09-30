#pragma once

#include "types.hpp"

namespace algorithm {

std::size_t find_smallest_factor(std::size_t n);

ComplexVector mixed_radix_fft(const ComplexVector& input);
ComplexVector mixed_radix_fft(const ComplexVector& input, std::size_t n1, std::size_t n2);

ComplexVector mixed_radix_ifft(const ComplexVector& input);

}  // namespace algorithm
