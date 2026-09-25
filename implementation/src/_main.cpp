#include <cmath>
#include <iomanip>
#include <iostream>
#include <string_view>

#include "fft/cooley_tukey.hpp"
#include "fft/naive_dft.hpp"
#include "fft/types.hpp"

namespace {

void print_complex_vector(std::string_view label, const fft::ComplexVector& vec) {
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
    std::cout << " Advanced Algorithms - PA1 C++ Setup    \n";
    std::cout << "========================================\n\n";

    const fft::ComplexVector input = {{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}};

    print_complex_vector("Input Signal", input);

    const auto dft_result = fft::naive_dft(input);
    print_complex_vector("DFT Result (Naive O(N^2))", dft_result);

    const auto fft_result = fft::cooley_tukey_fft(input);
    print_complex_vector("FFT Result (Cooley-Tukey O(N log N))", fft_result);

    const auto ifft_result = fft::cooley_tukey_ifft(fft_result);
    print_complex_vector("Reconstructed Signal (Cooley-Tukey IFFT)", ifft_result);

    std::cout << "Run successful!\n";
    return 0;
}
