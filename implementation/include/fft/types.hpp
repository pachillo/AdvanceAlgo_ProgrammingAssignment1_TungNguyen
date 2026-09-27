#pragma once

#include <complex>
#include <numbers>
#include <vector>
#include <cstddef>
#include <cmath>

namespace fft {

using Complex = std::complex<double>;
using ComplexVector = std::vector<Complex>;

inline constexpr double PI = std::numbers::pi;

}  // namespace fft
