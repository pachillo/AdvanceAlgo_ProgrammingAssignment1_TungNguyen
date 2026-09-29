#pragma once

#include "types.hpp"

namespace algorithm {
  bool is_power_of_two(std::size_t n);
  ComplexVector iterative_radix2_fft(const ComplexVector& input);
  ComplexVector iterative_radix2_ifft(const ComplexVector& input);
}
