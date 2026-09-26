
#include "fft/cooley_tukey.hpp"

#include <cmath>
#include <stdexcept>

namespace fft {

ComplexVector 2radix_cooley_tukey(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1) {
        return input;
    }

    if (!is_power_of_two(n)) {
        throw std::invalid_argument("Input size must be a power of 2 for radix-2 Cooley-Tukey FFT");
    }

    const std::size_t half = n / 2;
    ComplexVector even_samples(half);
    ComplexVector odd_samples(half);

    for (std::size_t i = 0; i < half; ++i) {
        even_samples[i] = input[2 * i];
        odd_samples[i] = input[2 * i + 1];
    }

    const ComplexVector e = cooley_tukey_fft(even_samples);
    const ComplexVector o = cooley_tukey_fft(odd_samples);

    ComplexVector res(n);
    const double n_double = static_cast<double>(n);

    for (std::size_t k = 0; k < half; ++k) {
        const double theta = -2.0 * PI * static_cast<double>(k) / n_double;
        const Complex twiddle = std::polar(1.0, theta);
        const Complex t = twiddle * o[k];

        res[k] = e[k] + t;
        res[k + half] = e[k] - t;
    }

    return res;
}

ComplexVector i2radix_cooley_tukey(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1) {
        return input;
    }

    if (!is_power_of_two(n)) {
        throw std::invalid_argument(
            "Input size must be a power of 2 for radix-2 Cooley-Tukey IFFT");
    }

    // Compute IFFT using conjugate property:
    // IFFT(x) = 1/N * conj(FFT(conj(x)))
    ComplexVector conjugated(n);
    for (std::size_t i = 0; i < n; ++i) {
        conjugated[i] = std::conj(input[i]);
    }

    ComplexVector transformed = cooley_tukey_fft(conjugated);

    const double n_double = static_cast<double>(n);
    for (std::size_t i = 0; i < n; ++i) {
        transformed[i] = std::conj(transformed[i]) / n_double;
    }

    return transformed;
}

}  // namespace fft
