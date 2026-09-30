#include "naive_dft.hpp"

namespace algorithm {

ComplexVector naive_dft(const ComplexVector& input) {
    // Step 1: Grab the input array length
    const std::size_t n = input.size();

    if (n == 0)
        return {};

    // Step 2: Allocate the result array
    ComplexVector res(n, Complex{0.0, 0.0});
    const double n_double = static_cast<double>(n);

    // Step 3: The main for loop of DFT
    for (std::size_t k = 0; k < n; ++k) {
        Complex sum{0.0, 0.0};
        const double k_double = static_cast<double>(k);

        for (std::size_t j = 0; j < n; ++j) {
            const double j_double = static_cast<double>(j);
            const double theta = -2.0 * PI * k_double * j_double / n_double;
            const Complex W = std::polar(1.0, theta);
            sum += input[j] * W;
        }
        res[k] = sum;
    }
    return res;
}

ComplexVector naive_idft(const ComplexVector& input) {
    const std::size_t n = input.size();

    if (n == 0)
        return {};

    ComplexVector res(n, Complex{0.0, 0.0});
    const double n_double = static_cast<double>(n);

    for (std::size_t j = 0; j < n; ++j) {
        Complex sum{0.0, 0.0};
        const double j_double = static_cast<double>(j);

        for (std::size_t k = 0; k < n; ++k) {
            const double k_double = static_cast<double>(k);
            const double theta =
                2.0 * PI * k_double * j_double / n_double;  // Remove the minus for IDFT
            const Complex W = std::polar(1.0, theta);
            sum += input[k] * W;
        }
        res[j] = sum / n_double;
    }
    return res;
}
}  // namespace algorithm
