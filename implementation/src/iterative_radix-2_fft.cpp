#include "iterative_radix-2_fft.hpp"

namespace algorithm {
  bool is_power_of_two(std::size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
  }
}
