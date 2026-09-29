#pragma once

#include "types.hpp"

namespace algorithm {
  ComplexVector naive_dft(const ComplexVector& input);
  ComplexVector naive_idft(const ComplexVector& input);
}
