#include "mixed-radix_fft.hpp"

#include "naive_dft.hpp"

namespace algorithm {

std::size_t find_smallest_factor(std::size_t n) {
    if (n <= 3)
        return n;
    if (n % 2 == 0)
        return 2;
    if (n % 3 == 0)
        return 3;

    // According to the division algorithm theorem,
    // for every 6 numbers, 4 of them are composite
    for (std::size_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0)
            return i;
        if (n % (i + 2) == 0)
            return i + 2;
    }
    return n;  // n is prime
}

ComplexVector mixed_radix_fft(const ComplexVector& input, std::size_t n1, std::size_t n2) {
    if (n1 == 0 || n2 == 0) {
        throw std::invalid_argument("Factors n1 and n2 must be greater than 0 for Mixed-Radix FFT");
    }
    const std::size_t n = n1 * n2;
    if (input.size() != n) {
        throw std::invalid_argument("Input size must equal n1 * n2 for Mixed-Radix FFT");
    }

    // Step 1: Map 1D input into N1 x N2 matrix A
    std::vector<ComplexVector> A(n1, ComplexVector(n2));
    for (std::size_t n1_idx = 0; n1_idx < n1; ++n1_idx) {
        for (std::size_t n2_idx = 0; n2_idx < n2; ++n2_idx) {
            A[n1_idx][n2_idx] = input[n1_idx * n2 + n2_idx];
        }
    }

    // Step 2: N2 independent column DFTs of size N1
    for (std::size_t n2_idx = 0; n2_idx < n2; ++n2_idx) {
        ComplexVector col(n1);
        for (std::size_t n1_idx = 0; n1_idx < n1; ++n1_idx) {
            col[n1_idx] = A[n1_idx][n2_idx];
        }
        ComplexVector col_dft = mixed_radix_fft(col);
        for (std::size_t k1 = 0; k1 < n1; ++k1) {
            A[k1][n2_idx] = col_dft[k1];
        }
    }

    // Step 3: Pointwise twiddle factor multiplication evaluated on the fly
    const double n_double = static_cast<double>(n);
    for (std::size_t k1 = 0; k1 < n1; ++k1) {
        for (std::size_t n2_idx = 0; n2_idx < n2; ++n2_idx) {
            const double theta = -2.0 * PI * static_cast<double>(k1 * n2_idx) / n_double;
            const Complex W = std::polar(1.0, theta);
            A[k1][n2_idx] *= W;
        }
    }

    // Step 4: N1 independent row DFTs of size N2
    std::vector<ComplexVector> B(n1, ComplexVector(n2));
    for (std::size_t k1 = 0; k1 < n1; ++k1) {
        ComplexVector row = A[k1];
        ComplexVector row_dft = mixed_radix_fft(row);
        for (std::size_t k2 = 0; k2 < n2; ++k2) {
            B[k1][k2] = row_dft[k2];
        }
    }

    // Step 5: Read out transposed matrix into 1D output
    ComplexVector res(n);
    for (std::size_t k2 = 0; k2 < n2; ++k2) {
        for (std::size_t k1 = 0; k1 < n1; ++k1) {
            res[k2 * n1 + k1] = B[k1][k2];
        }
    }

    return res;
}

// Wrapper for convenience
ComplexVector mixed_radix_fft(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1)
        return input;

    const std::size_t n1 = find_smallest_factor(n);
    // Base case: if n is prime, cannot factor further
    if (n1 == n) {
        return naive_dft(input);
    }

    const std::size_t n2 = n / n1;
    return mixed_radix_fft(input, n1, n2);
}

ComplexVector mixed_radix_ifft(const ComplexVector& input) {
    const std::size_t n = input.size();
    if (n <= 1)
        return input;

    ComplexVector conjugated(n);
    for (std::size_t i = 0; i < n; ++i) {
        conjugated[i] = std::conj(input[i]);
    }

    ComplexVector res = mixed_radix_fft(conjugated);

    const double n_double = static_cast<double>(n);
    for (std::size_t i = 0; i < n; ++i) {
        res[i] = std::conj(res[i]) / n_double;
    }
    return res;
}

}  // namespace algorithm
