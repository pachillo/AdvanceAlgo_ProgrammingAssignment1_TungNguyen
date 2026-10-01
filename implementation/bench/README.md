# Benchmark Suite (`implementation/bench`)

This directory provides high-precision C++ benchmarking and Python visualization scripts for all six experimental studies in **Programming Assignment 1: Fast Fourier Transform**.

## Experiments Overview

| Experiment | Focus | Command | Output Plot |
| :--- | :--- | :--- | :--- |
| **Exp 1** | Asymptotic Scaling & Crossover Point (Naive DFT vs. Recursive FFT) | `make bench-all` | `experiment1_loglog.png` |
| **Exp 2** | In-Place Speedup (Recursive vs. Iterative Radix-2 FFT) | `make bench2-all` | `experiment2_speedup.png` |
| **Exp 3** | Trigonometric Math vs. Cache Lookup (Iterative vs. Precomputed Twiddles) | `make bench3-all` | `experiment3_precomputed.png` |
| **Exp 4** | The Hardware "Cache Cliff" Effect ($\tau(N)$ vs. L1/L2/L3 Caches) | `make bench4-all` | `experiment4_cache_cliff.png` |
| **Exp 5** | Numerical Precision Drift (Reconstruction Error $\|x - \text{IFFT}(\text{FFT}(x))\|_\infty$) | `make bench5-all` | `experiment5_precision.png` |
| **Exp 6** | Mixed-Radix Factorisation vs. Prime Degradation (Composite vs. Primes) | `make bench6-all` | `experiment6_mixed_radix.png` |

## Directory Structure

```text
implementation/bench/
├── bench_exp1.cpp ... exp6.cpp # High-precision C++ runners (std::chrono, memory barriers)
├── plot_exp1.py ... exp6.py    # Python analysis & visualization (matplotlib, scipy)
├── requirements.txt            # Python dependencies (matplotlib, numpy, scipy)
├── results/                    # Generated CSV data and high-resolution PNG plots
└── README.md
```

## Quick Start (via Makefile)

From `implementation/`, run:

```bash
# Run all 6 benchmarks and generate all figures at once:
make bench-suite

# Or run individual experiment pipelines:
make bench-all   # Experiment 1
make bench2-all  # Experiment 2
make bench3-all  # Experiment 3
make bench4-all  # Experiment 4
make bench5-all  # Experiment 5
make bench6-all  # Experiment 6
```

Plots are saved to `bench/results/` and automatically mirrored to `report/` for inclusion in the LaTeX report.
