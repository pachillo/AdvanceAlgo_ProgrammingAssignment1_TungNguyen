#!/usr/bin/env python3
"""
Experiment 3: Trigonometric Math vs. Cache Lookup (Iterative vs. Precomputed Twiddles)
Plots:
  Panel 1: Execution Time vs. N (Log-Log Scale)
  Panel 2: Speedup Factor (T_iterative / T_precomputed) vs. N
Highlights L1/L2 cache sweet spot and the cache-miss penalty at huge N (>= 2^18).
Outputs PNG only (no PDF).
"""

import sys
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def main():
    bench_dir = Path(__file__).resolve().parent
    default_csv = bench_dir / "results" / "experiment3.csv"
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else default_csv

    if not csv_path.exists():
        print(f"Error: CSV file not found at {csv_path}")
        print("Please run the benchmark first (e.g. 'make bench3').")
        sys.exit(1)

    # Load data
    data = np.genfromtxt(csv_path, delimiter=",", names=True)
    n = data["N"]
    iter_time_ns = data["Iterative_mean_ns"]
    pre_time_ns = data["Precomputed_mean_ns"]
    speedup = data["Speedup_Factor"]

    # Convert to microseconds
    iter_time_us = iter_time_ns / 1000.0
    pre_time_us = pre_time_ns / 1000.0

    print("==================================================")
    print(" Experiment 3: Precomputed Twiddles Summary       ")
    print("==================================================")
    print(f"Min Speedup: {np.min(speedup):.2f}x (at N = {int(n[np.argmin(speedup)])})")
    print(f"Max Speedup: {np.max(speedup):.2f}x (at N = {int(n[np.argmax(speedup)])})")
    print(f"Average Speedup (N <= 2^16): {np.mean(speedup[n <= 65536]):.2f}x")
    print(f"Speedup at N = 2^18: {speedup[n == 262144][0]:.2f}x")
    print(f"Speedup at N = 2^20: {speedup[n == 1048576][0]:.2f}x")
    print("==================================================\n")

    # Set up matplotlib styling
    plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8.5), dpi=300, sharex=True)

    # -------------------------------------------------------------
    # Panel 1: Execution Time vs N (Log-Log)
    # -------------------------------------------------------------
    ax1.plot(
        n, iter_time_us,
        marker="o", markersize=6, linewidth=2, color="#c0392b",
        label="Iterative (On-the-fly $\\mathrm{std::polar}$)"
    )
    ax1.plot(
        n, pre_time_us,
        marker="s", markersize=6, linewidth=2, color="#2980b9",
        label="Precomputed (Table Lookup)"
    )

    ax1.set_xscale("log", base=2)
    ax1.set_yscale("log")
    ax1.set_ylabel("Execution Time ($\\mu$s) [Log Scale]", fontsize=11, fontweight="bold")
    ax1.set_title(
        "Experiment 3: Trigonometric Math vs. Cache Lookup\nIterative FFT vs. Precomputed Twiddles FFT ($N = 2^1$ to $2^{20}$)",
        fontsize=12,
        fontweight="bold",
        pad=10
    )
    ax1.grid(True, which="both", ls=":", alpha=0.6)
    ax1.legend(loc="upper left", frameon=True, framealpha=0.95, fontsize=10)

    # -------------------------------------------------------------
    # Panel 2: Speedup Factor vs N
    # -------------------------------------------------------------
    ax2.plot(
        n, speedup,
        marker="D", markersize=6, linewidth=2.3, color="#16a085",
        label="Speedup Factor ($T_{\\mathrm{iterative}} / T_{\\mathrm{precomputed}}$)"
    )
    ax2.axhline(1.0, color="#7f8c8d", linestyle="--", linewidth=1.2, label="Parity ($1.0\\times$)")

    # Shaded band for high-memory cache-spill zone (N >= 2^18, Table >= 2MB)
    ax2.axvspan(
        262144, 1048576,
        color="#fadbd8", alpha=0.45,
        label="Cache-Spill Zone ($N \\geq 2^{18}$, Table $\\geq 2$MB$-$8MB)"
    )

    # Annotate peak speedup
    max_idx = np.argmax(speedup)
    max_n = int(n[max_idx])
    max_val = speedup[max_idx]
    ax2.annotate(
        f"Peak Speedup: {max_val:.2f}$\\times$\n($N = {max_n:,}$)",
        xy=(max_n, max_val),
        xytext=(max_n * 3.5, max_val * 0.85),
        arrowprops=dict(facecolor="#16a085", shrink=0.08, width=1.5, headwidth=6),
        fontsize=9,
        fontweight="bold",
        color="#0e6251",
        bbox=dict(boxstyle="round,pad=0.3", fc="#e8f8f5", ec="#16a085", alpha=0.9)
    )

    # Annotate behavior at huge N
    last_n = int(n[-1])
    last_val = speedup[-1]
    ax2.annotate(
        f"Cache Eviction Zone:\n{last_val:.2f}$\\times$ at $N = 1$M",
        xy=(last_n, last_val),
        xytext=(last_n * 0.15, last_val + 0.5),
        arrowprops=dict(facecolor="#c0392b", shrink=0.08, width=1.5, headwidth=6),
        fontsize=9,
        fontweight="bold",
        color="#922b21",
        bbox=dict(boxstyle="round,pad=0.3", fc="#fdedec", ec="#c0392b", alpha=0.9)
    )

    ax2.set_xscale("log", base=2)
    ax2.set_xticks(n)

    # Format tick labels
    xtick_labels = []
    for val in n:
        pow2 = int(np.log2(val))
        if val >= 1048576:
            s = f"$2^{{{pow2}}}$\n(1M)"
        elif val >= 1024:
            s = f"$2^{{{pow2}}}$\n({int(val/1024)}k)"
        else:
            s = f"$2^{{{pow2}}}$\n({int(val)})"
        xtick_labels.append(s)

    ax2.set_xticklabels(xtick_labels, fontsize=7.5)
    ax2.set_xlabel("Input Size ($N$) [Log2 Scale]", fontsize=11, fontweight="bold")
    ax2.set_ylabel("Speedup Factor ($T_{\\mathrm{iter}} / T_{\\mathrm{pre}}$)", fontsize=11, fontweight="bold")

    y_top = max(3.0, np.max(speedup) + 0.6)
    ax2.set_ylim(bottom=0.0, top=y_top)

    ax2.grid(True, which="both", ls=":", alpha=0.6)
    ax2.legend(loc="upper left", frameon=True, framealpha=0.95, fontsize=9.5)

    fig.tight_layout()

    # Destination directories
    out_dir = bench_dir / "results"
    out_dir.mkdir(parents=True, exist_ok=True)
    report_dir = bench_dir.parent.parent / "report"

    # Save to bench/results/ (PNG only)
    png_bench = out_dir / "experiment3_precomputed.png"
    plt.savefig(png_bench, dpi=300)
    print(f"Saved plot to: {png_bench}")

    # Copy to report/ (PNG only)
    if report_dir.exists():
        png_report = report_dir / "experiment3_precomputed.png"
        plt.savefig(png_report, dpi=300)
        print(f"Copied report plot to: {png_report}")

if __name__ == "__main__":
    main()
