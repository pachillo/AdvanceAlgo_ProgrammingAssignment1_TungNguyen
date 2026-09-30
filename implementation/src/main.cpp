#include <cmath>
#include <iomanip>
#include <iostream>
#include <string_view>

#include "naive_dft.hpp"
#include "recursive_radix-2_fft.hpp"
#include "types.hpp"

namespace {

void print_complex_vector(std::string_view label, const algorithm::ComplexVector& vec) {
    std::cout << label << ":\n";
    for (std::size_t i = 0; i < vec.size(); ++i) {
        const double re = vec[i].real();
        const double im = vec[i].imag();
        const char sign = (im >= 0.0) ? '+' : '-';
        std::cout << "  [" << i << "] " << std::fixed << std::setprecision(4) << re << " " << sign
                  << " " << std::abs(im) << "i\n";
    }
    std::cout << '\n';
}

}  // namespace

int main() {
    std::cout << "========================================\n";
    std::cout << " Fast Fourier Transform   \n";
    std::cout << "========================================\n\n";

    // Try with your input array here
    const algorithm::ComplexVector input = {{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}};

    print_complex_vector("Input Signal", input);

    const auto dft_result = algorithm::naive_dft(input);
    print_complex_vector("DFT Result (Naive O(N^2))", dft_result);

    const auto fft_result = algorithm::recursive_radix2_fft(input);
    print_complex_vector("FFT Result (Recursive Cooley-Tukey O(N log N))", fft_result);

    const auto ifft_result = algorithm::recursive_radix2_ifft(fft_result);
    print_complex_vector("Reconstructed Signal (Recursive IFFT)", ifft_result);

    std::cout << "Run successful!\n";
    return 0;
}
