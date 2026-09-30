#include "recursive_radix-2_fft.hpp"

namespace algorithm {

ComplexVector recursive_radix2_fft(const ComplexVector& input) {
    // Step 1: Grab the input size
    const std::size_t n = input.size();
    if (n <= 1) {
        return input;
    }

    // This only work if the input size is 2^l
    if (!is_power_of_two(n)) {
        throw std::invalid_argument("Input size must be a power of 2 for Radix-2 FFT");
    }

    // Step 2: Splitting the input array in half
    const std::size_t half = n / 2;
    ComplexVector even_samples(half);
    ComplexVector odd_samples(half);

    // The even get the even index elements
    // The odd get the odd index elements
    for (std::size_t i = 0; i < half; ++i) {
        even_samples[i] = input[2 * i];
        odd_samples[i] = input[2 * i + 1];
    }

    // Recursively call to fill out the arrays
    const ComplexVector e = recursive_radix2_fft(even_samples);
    const ComplexVector o = recursive_radix2_fft(odd_samples);

    // Prepare the return array
    ComplexVector res(n);
    const double n_double = static_cast<double>(n);

    // Step 3: The main for loop of this FFT
    for (std::size_t k = 0; k < half; ++k) {
        const double theta = -2.0 * PI * static_cast<double>(k) / n_double;
        const Complex W = std::polar(1.0, theta);
        const Complex t = W * o[k];

        res[k] = e[k] + t;
        res[k + half] = e[k] - t;
    }
    return res;
}

ComplexVector recursive_radix2_ifft(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1) {
        return input;
    }

    if (!is_power_of_two(n)) {
        throw std::invalid_argument("Input size must be a power of 2 for Radix-2 IFFT");
    }

    ComplexVector conjugated(n);
    for (std::size_t i = 0; i < n; ++i) {
        conjugated[i] = std::conj(input[i]);
    }

    ComplexVector res = recursive_radix2_fft(conjugated);

    const double n_double = static_cast<double>(n);
    for (std::size_t i = 0; i < n; ++i) {
        res[i] = std::conj(res[i]) / n_double;
    }
    return res;
}
}  // namespace algorithm
