#pragma once

#include "types.hpp"

namespace algorithm {
ComplexVector recursive_radix2_fft(const ComplexVector& input);
ComplexVector recursive_radix2_ifft(const ComplexVector& input);
}  // namespace algorithm
