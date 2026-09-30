#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace algorithm {
using Complex = std::complex<double>;  // Holds 2 double values for the real and imaginary part of a
                                       // complex number
using ComplexVector = std::vector<Complex>;
inline constexpr double PI = std::numbers::pi;

inline bool is_power_of_two(std::size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}
}  // namespace algorithm
