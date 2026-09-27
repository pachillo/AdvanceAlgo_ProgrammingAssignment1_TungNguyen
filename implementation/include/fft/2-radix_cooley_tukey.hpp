#pragma once

#include "fft/types.hpp"

namespace fft {

bool is_power_of_two(std::size_t n);

ComplexVector 2radix_cooley_tukey(const ComplexVector& input);

ComplexVector i2radix_cooley_tukey(const ComplexVector& input);

}  // namespace fft
