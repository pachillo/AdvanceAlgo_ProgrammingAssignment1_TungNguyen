#pragma once

#include <complex>
#include <numbers>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <cmath>

namespace algorithm {
using Complex = std::complex<double>; // Holds 2 double values for the real and imaginary part of a complex number
using ComplexVector = std::vector<Complex>;
inline constexpr double PI = std::numbers::pi;
}
