# Fast Fourier Transform (FFT) — Implementation & Empirical Study

**Course:** 41052 Advanced Algorithms — Programming Assignment 1  
**Track:** Track A (Implementation and Empirical Study)  
**Author:** Tung Nguyen (Student ID: 24666048)  
**Email:** <Tung.D.Nguyen@student.uts.edu.au>  

---

## Overview

This repository contains high-performance C++20 implementations of the **Discrete Fourier Transform (DFT)** and **Fast Fourier Transform (FFT)**, alongside an automated benchmarking suite evaluating six empirical studies:

1. **Naive DFT & IDFT** ($\mathcal{O}(N^2)$ direct summation baseline)
2. **Recursive Radix-2 Cooley–Tukey FFT & IFFT** ($\mathcal{O}(N \log N)$ divide-and-conquer)
3. **Iterative In-Place Radix-2 FFT & IFFT** ($\mathcal{O}(N \log N)$ in-place bit-reversal)
4. **Iterative Radix-2 with Precomputed Twiddle Table** ($\mathcal{O}(N \log N)$ table lookup)
5. **Mixed-Radix General Cooley–Tukey FFT & IFFT** (2D composite tensor decomposition)

---

## Prerequisites

* **C++ Compiler:** GCC 14+ or Clang 16+ (supporting **C++20**)
* **Build System:** CMake $\ge 3.20$ and GNU Make
* **Python:** Python 3.10+ (for benchmark plotting)

---

## Quick Start (Building & Running)

All tasks are automated via the `Makefile` inside `implementation/`:

```bash
cd implementation

# 1. Build all targets (Release mode with -O3)
make build

# 2. Run demonstration application
make run

# 3. Run automated verification test suite
make test
```

---

## Reproducing the Empirical Study (Track A Artifact)

To run all 6 benchmark experiments and regenerate the report figures:

```bash
cd implementation

# Run all 6 benchmarks and plot figures (creates venv and installs dependencies)
make bench-suite
```

### Individual Experiment Targets

| Target | Description | Output Plot |
| :--- | :--- | :--- |
| `make bench-all` | **Exp 1:** Asymptotic Scaling & Crossover ($N = 2^1 \dots 2^{13}$) | `experiment1_loglog.png` |
| `make bench2-all` | **Exp 2:** Stack Overhead & In-Place Speedup ($N = 2^1 \dots 2^{18}$) | `experiment2_speedup.png` |
| `make bench3-all` | **Exp 3:** On-the-Fly Math vs. Precomputed Twiddles ($N = 2^1 \dots 2^{20}$) | `experiment3_precomputed.png` |
| `make bench4-all` | **Exp 4:** Hardware Cache Cliff Analysis ($\tau(N)$ vs. L1/L2/L3) | `experiment4_cache_cliff.png` |
| `make bench5-all` | **Exp 5:** Numerical Precision & Roundoff Drift ($\|e\|_\infty$ and RMSE) | `experiment5_precision.png` |
| `make bench6-all` | **Exp 6:** Mixed-Radix Factorisation vs. Prime Degradation | `experiment6_mixed_radix.png` |

*Raw CSV data and plots are saved in `implementation/bench/results/` and mirrored to `report/`.*

---

## Repository Structure

```text
.
├── README.md                          # Top-level reproduction guide
├── implementation/
│   ├── CMakeLists.txt                 # CMake configuration (C++20, -O3)
│   ├── Makefile                       # Top-level build and benchmark automation
│   ├── include/                       # Public C++ headers (types.hpp, algorithm declarations)
│   ├── src/                           # Concrete C++ algorithm implementations
│   ├── tests/
│   │   └── test.cpp                   # Automated correctness assertions & edge case tests
│   └── bench/                         # Benchmarking suite
│       ├── bench_exp1.cpp ... exp6.cpp# C++ monotonic benchmark runners
│       ├── plot_exp1.py ... exp6.py   # Python plotting scripts (matplotlib, scipy)
│       ├── requirements.txt           # Python dependencies
│       └── results/                   # Benchmark CSV datasets and PNG figures
└── report/
    ├── ProgrammingAssignment1_TungNguyen_Report.tex  # LaTeX report source
    ├── ProgrammingAssignment1_TungNguyen_Report.pdf  # Compiled PDF report
    ├── ref.bib                        # Bibliography
    └── *.png                          # Benchmark figures
    └── other files are just build file, can be ignored
```
