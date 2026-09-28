#pragma once

#include "fft/types.hpp"

namespace fft {

std::size_t find_smallest_factor(std::size_t n);

ComplexVector cooley_tukey_fft(const ComplexVector& input);

ComplexVector cooley_tukey_ifft(const ComplexVector& input);

}  // namespace fft
