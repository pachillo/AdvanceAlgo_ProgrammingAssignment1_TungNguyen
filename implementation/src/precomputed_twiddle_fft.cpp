#include "precomputed_twiddle_fft.hpp"

#include "iterative_radix-2_fft.hpp"

namespace algorithm {

ComplexVector precompute_twiddles(std::size_t n) {
    const std::size_t half_n = n / 2;
    ComplexVector res(half_n);
    const double n_double = static_cast<double>(n);

    // This is ran in the main loop of the iterative radix-2 FFT
    // Now it's run here beforehand
    for (std::size_t k = 0; k < half_n; ++k) {
        const double theta = -2.0 * PI * static_cast<double>(k) / n_double;
        res[k] = std::polar(1.0, theta);
    }
    return res;
}

ComplexVector precompute_twiddles_for_ifft(std::size_t n) {
    const std::size_t half_n = n / 2;
    ComplexVector res(half_n);
    const double n_double = static_cast<double>(n);

    for (std::size_t k = 0; k < half_n; ++k) {
        const double theta = 2.0 * PI * static_cast<double>(k) / n_double;  // Inverse operator
        res[k] = std::polar(1.0, theta);
    }
    return res;
}

ComplexVector precompute_twiddles_ifft(std::size_t n) {
    return precompute_twiddles_for_ifft(n);
}

ComplexVector precomputed_twiddle_fft(const ComplexVector& input, const ComplexVector& W) {
    // Step 1: Grab the input array length
    const std::size_t n = input.size();
    if (n <= 1)
        return input;

    // This can only work if the array size is 2^m
    if (!is_power_of_two(n)) {
        throw std::invalid_argument("Input array size must be a power of 2 for Radix-2 FFT");
    }

    // Step 2: Perform bit reverse permutation
    ComplexVector res = bit_reverse_permutation(input);
    const int l = static_cast<int>(std::log2(static_cast<double>(n)));

    // Step 3: The main for loop of this FFT
    for (int s = 1; s <= l; ++s) {
        const std::size_t m = static_cast<std::size_t>(1) << s;
        const std::size_t step = n / m;

        for (size_t k = 0; k < n; k += m) {
            for (size_t j = 0; j < m / 2; ++j) {
                const Complex W_val = W[j * step];  // The twiddle is already computed
                const Complex t = W_val * res[k + j + m / 2];
                const Complex u = res[k + j];
                res[k + j] = u + t;
                res[k + j + m / 2] = u - t;
            }
        }
    }
    return res;
}

// A wrapper to conveniently call the above function
ComplexVector precomputed_twiddle_fft(const ComplexVector& input) {
    if (input.size() <= 1)
        return input;
    const ComplexVector W = precompute_twiddles(input.size());
    return precomputed_twiddle_fft(input, W);
}

ComplexVector precomputed_twiddle_ifft(const ComplexVector& input, const ComplexVector& W) {
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
        const std::size_t step = n / m;

        for (size_t k = 0; k < n; k += m) {
            for (size_t j = 0; j < m / 2; ++j) {
                const Complex W_val = W[j * step];
                const Complex t = W_val * res[k + j + m / 2];
                const Complex u = res[k + j];
                res[k + j] = u + t;
                res[k + j + m / 2] = u - t;
            }
        }
    }

    const double n_double = static_cast<double>(n);
    for (size_t i = 0; i < n; ++i) {
        res[i] /= n_double;
    }
    return res;
}

// A wrapper to conveniently call the above function
ComplexVector precomputed_twiddle_ifft(const ComplexVector& input) {
    if (input.size() <= 1)
        return input;
    const ComplexVector W = precompute_twiddles_for_ifft(input.size());
    return precomputed_twiddle_ifft(input, W);
}

}  // namespace algorithm
