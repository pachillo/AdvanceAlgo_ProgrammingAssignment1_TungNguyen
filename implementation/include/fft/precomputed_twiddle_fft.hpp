#pragma once

#include "fft/types.hpp"

namespace fft {

ComplexVector precomputed_twiddle_fft(const ComplexVector& input);

ComplexVector precomputed_twiddle_ifft(const ComplexVector& input);

}  // namespace fft
