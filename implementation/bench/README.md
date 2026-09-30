# Benchmark Suite (`implementation/bench`)

This directory provides high-precision C++ benchmarking and Python visualization scripts for all five experimental studies in **Programming Assignment 1: Fast Fourier Transform**.

## Experiments Overview

| Experiment | Focus | Command | Output Plot |
| :--- | :--- | :--- | :--- |
| **Exp 1** | Asymptotic Scaling & Crossover Point (Naive DFT vs. Recursive FFT) | `make bench-all` | `experiment1_loglog.png` |
| **Exp 2** | In-Place Speedup (Recursive vs. Iterative Radix-2 FFT) | `make bench2-all` | `experiment2_speedup.png` |
| **Exp 3** | Trigonometric Math vs. Cache Lookup (Iterative vs. Precomputed Twiddles) | `make bench3-all` | `experiment3_precomputed.png` |
| **Exp 4** | The Hardware "Cache Cliff" Effect ($\tau(N)$ vs. L1/L2/L3 Caches) | `make bench4-all` | `experiment4_cache_cliff.png` |
| **Exp 5** | Numerical Precision Drift (Reconstruction Error $\|x - \text{IFFT}(\text{FFT}(x))\|_\infty$) | `make bench5-all` | `experiment5_precision.png` |

## Directory Structure

```text
implementation/bench/
├── bench_exp1.cpp       # Exp 1 C++ runner (std::chrono, warmups, adaptive loops)
├── bench_exp2.cpp       # Exp 2 C++ runner (speedup ratio measurement)
├── bench_exp3.cpp       # Exp 3 C++ runner (trig vs precomputed twiddles)
├── bench_exp4.cpp       # Exp 4 C++ runner (normalized time tau(N) vs cache hierarchy)
├── bench_exp5.cpp       # Exp 5 C++ runner (round-trip Linf and RMSE error metrics)
├── plot_exp1.py         # Exp 1 Python analysis (slopes, crossover, log-log)
├── plot_exp2.py         # Exp 2 Python analysis (speedup ratio vs. N)
├── plot_exp3.py         # Exp 3 Python analysis (2-panel execution time & speedup)
├── plot_exp4.py         # Exp 4 Python analysis (tau(N) vs L1/L2/L3 cache cliffs)
├── plot_exp5.py         # Exp 5 Python analysis (semi-log Linf error vs. N)
├── requirements.txt     # Python dependencies (matplotlib, numpy, scipy)
├── results/             # Generated CSV data and high-resolution PNG plots
└── README.md
```

## Quick Start (via Makefile)

From `implementation/`, run:

```bash
# Run all steps for any specific experiment:
make bench-all   # Experiment 1
make bench2-all  # Experiment 2
make bench3-all  # Experiment 3
make bench4-all  # Experiment 4
make bench5-all  # Experiment 5

# Or run individual steps:
make bench5-build   # Build the C++ benchmark executable
make bench5         # Run benchmark and output results/experiment5.csv
make bench-env      # Set up Python virtual environment (.venv)
make bench5-plot    # Run plot_exp5.py using .venv to generate plots
```

Plots are saved to `bench/results/` and automatically mirrored to `report/` (PNG format only) for seamless inclusion in the LaTeX report.
