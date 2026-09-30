cmake_minimum_required(VERSION 3.20)
project(FFT_PA1 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for LSP (clangd) support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release mode with optimizations if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Compiler warnings and options
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
)

# Core FFT library
add_library(fft_lib
    src/naive_dft.cpp
    src/recursive_radix-2_fft.cpp
    src/iterative_radix-2_fft.cpp
    src/mixed-radix_fft.cpp
    src/precomputed_twiddle_fft.cpp
)

target_include_directories(fft_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Main demonstration application
add_executable(fft_app src/main.cpp)
target_link_libraries(fft_app PRIVATE fft_lib)

# Testing
enable_testing()
add_executable(fft_test tests/test.cpp)
target_link_libraries(fft_test PRIVATE fft_lib)
add_test(NAME FFT_Verification_Tests COMMAND fft_test)

# Benchmarks
add_executable(fft_bench_exp1 bench/bench_exp1.cpp)
target_link_libraries(fft_bench_exp1 PRIVATE fft_lib)

add_executable(fft_bench_exp2 bench/bench_exp2.cpp)
target_link_libraries(fft_bench_exp2 PRIVATE fft_lib)

add_executable(fft_bench_exp3 bench/bench_exp3.cpp)
target_link_libraries(fft_bench_exp3 PRIVATE fft_lib)

add_executable(fft_bench_exp4 bench/bench_exp4.cpp)
target_link_libraries(fft_bench_exp4 PRIVATE fft_lib)

add_executable(fft_bench_exp5 bench/bench_exp5.cpp)
target_link_libraries(fft_bench_exp5 PRIVATE fft_lib)
