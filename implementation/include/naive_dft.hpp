#pragma once

#include "fft/types.hpp"

namespace fft {

ComplexVector naive_dft(const ComplexVector& input);

ComplexVector naive_idft(const ComplexVector& input);

}  // namespace fft
