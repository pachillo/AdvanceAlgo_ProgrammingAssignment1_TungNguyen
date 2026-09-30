#include "iterative_radix-2_fft.hpp"

namespace algorithm {

ComplexVector bit_reverse_permutation(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1) {
        return input;
    }

    ComplexVector res = input;
    const std::size_t m = static_cast<std::size_t>(std::log2(static_cast<double>(n)));

    for (std::size_t i = 0; i < n; ++i) {
        std::size_t rev = 0;
        std::size_t temp = i;

        for (std::size_t b = 0; b < m; ++b) {
            std::size_t bit = temp % 2;
            rev = 2 * rev + bit;
            temp /= 2;
        }

        if (i < rev)
            std::swap(res[i], res[rev]);  // Only swap once to avoid undoing
    }

    return res;
}

ComplexVector iterative_radix2_fft(const ComplexVector& input) {
    // Step 1: Grab the input array length
    const std::size_t n = input.size();
    if (n <= 1)
        return input;

    // This only work if size = 2^m
    if (!is_power_of_two(n)) {
        throw std::invalid_argument("Input array size must be a power of 2 for Radix-2 FFT");
    }

    // Step 2: Perform bit reverse permutation
    ComplexVector res = bit_reverse_permutation(input);
    const int l = static_cast<int>(std::log2(static_cast<double>(n)));

    // Step 3: The main for loop of this FFT
    for (int s = 1; s <= l; ++s) {
        const std::size_t m = static_cast<std::size_t>(1) << s;
        const double theta = -2.0 * PI / static_cast<double>(m);
        const Complex W_m = std::polar(1.0, theta);

        for (size_t k = 0; k < n; k += m) {
            Complex W = 1.0;

            for (size_t j = 0; j < m / 2; ++j) {
                const Complex t = W * res[k + j + m / 2];
                const Complex u = res[k + j];
                res[k + j] = u + t;
                res[k + j + m / 2] = u - t;
                W = W * W_m;
            }
        }
    }
    return res;
}

ComplexVector iterative_radix2_ifft(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1)
        return input;

    if (!is_power_of_two(n)) {
        throw std::invalid_argument("Input array size must be a power of 2 for Radix-2 IFFT");
    }

    ComplexVector res = bit_reverse_permutation(input);
    const int l = static_cast<int>(std::log2(static_cast<double>(n)));

    for (int s = 1; s <= l; ++s) {
        const std::size_t m = static_cast<std::size_t>(1) << s;
        const double theta = 2.0 * PI / static_cast<double>(m);  // Inverse the operator
        const Complex W_m = std::polar(1.0, theta);
        for (size_t k = 0; k < n; k += m) {
            Complex W = 1.0;
            for (size_t j = 0; j < m / 2; ++j) {
                const Complex t = W * res[k + j + m / 2];
                const Complex u = res[k + j];
                res[k + j] = u + t;
                res[k + j + m / 2] = u - t;
                W = W * W_m;
            }
        }
    }
    const double n_double = static_cast<double>(n);
    for (size_t i = 0; i < n; ++i) {
        res[i] /= n_double;
    }
    return res;
}
}  // namespace algorithm
