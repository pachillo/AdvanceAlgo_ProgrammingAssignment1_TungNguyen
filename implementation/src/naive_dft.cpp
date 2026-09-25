#include "fft/naive_dft.hpp"

#include <cmath>

namespace fft {

ComplexVector naive_dft(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n == 0) {
        return {};
    }

    ComplexVector output(n, Complex{0.0, 0.0});
    const double n_double = static_cast<double>(n);

    for (std::size_t k = 0; k < n; ++k) {
        Complex sum{0.0, 0.0};
        const double k_double = static_cast<double>(k);

        for (std::size_t j = 0; j < n; ++j) {
            const double j_double = static_cast<double>(j);
            const double angle = -2.0 * PI * k_double * j_double / n_double;
            const Complex twiddle = std::polar(1.0, angle);
            sum += input[j] * twiddle;
        }
        output[k] = sum;
    }

    return output;
}

ComplexVector naive_idft(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n == 0) {
        return {};
    }

    ComplexVector output(n, Complex{0.0, 0.0});
    const double n_double = static_cast<double>(n);

    for (std::size_t j = 0; j < n; ++j) {
        Complex sum{0.0, 0.0};
        const double j_double = static_cast<double>(j);

        for (std::size_t k = 0; k < n; ++k) {
            const double k_double = static_cast<double>(k);
            const double angle = 2.0 * PI * k_double * j_double / n_double;
            const Complex twiddle = std::polar(1.0, angle);
            sum += input[k] * twiddle;
        }
        output[j] = sum / n_double;
    }

    return output;
}

}  // namespace fft
