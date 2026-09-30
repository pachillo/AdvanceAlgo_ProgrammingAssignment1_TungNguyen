#!/usr/bin/env python3
"""
Experiment 2: The In-Place Speedup (Recursive vs. Iterative Radix-2 FFT)
Plots Speedup Ratio (T_recursive / T_iterative) vs. N on a semi-log scale (N on log2 scale).
Highlights the 4x-8x hypothesis zone and cache/allocation overhead scaling.
Outputs PNG only (no PDF).
"""

import sys
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def main():
    bench_dir = Path(__file__).resolve().parent
    default_csv = bench_dir / "results" / "experiment2.csv"
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else default_csv

    if not csv_path.exists():
        print(f"Error: CSV file not found at {csv_path}")
        print("Please run the benchmark first (e.g. 'make bench2').")
        sys.exit(1)

    # Load data
    data = np.genfromtxt(csv_path, delimiter=",", names=True)
    n = data["N"]
    rec_time_ns = data["Recursive_mean_ns"]
    iter_time_ns = data["Iterative_mean_ns"]
    speedup = data["Speedup_Ratio"]

    print("==================================================")
    print(" Experiment 2: Speedup Ratio Summary              ")
    print("==================================================")
    print(f"Min Speedup: {np.min(speedup):.2f}x (at N = {int(n[np.argmin(speedup)])})")
    print(f"Max Speedup: {np.max(speedup):.2f}x (at N = {int(n[np.argmax(speedup)])})")
    print(f"Mean Speedup (N >= 64): {np.mean(speedup[n >= 64]):.2f}x")
    print("==================================================\n")

    # Set up matplotlib plot styling
    plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
    fig, ax = plt.subplots(figsize=(9.5, 5.5), dpi=300)

    # Shaded band for hypothesized 4x - 8x speedup
    ax.axhspan(4.0, 8.0, color="#fff3cd", alpha=0.6, label="Hypothesized Speedup Range ($4\\times - 8\\times$)")
    ax.axhline(1.0, color="#7f8c8d", linestyle="--", linewidth=1.2, label="Parity ($1.0\\times$)")

    # Plot Speedup Ratio vs N
    ax.plot(
        n, speedup,
        marker="o", markersize=7, linewidth=2.5, color="#1a5276",
        label="Measured Speedup ($T_{\\text{recursive}} / T_{\\text{iterative}}$)"
    )

    # Annotate peak speedup
    max_idx = np.argmax(speedup)
    max_n = int(n[max_idx])
    max_val = speedup[max_idx]
    ax.annotate(
        f"Peak Speedup: {max_val:.2f}$\\times$\n($N = {max_n:,}$)",
        xy=(max_n, max_val),
        xytext=(max_n * 0.25, max_val + 0.5),
        arrowprops=dict(facecolor="#c0392b", shrink=0.08, width=1.5, headwidth=7),
        fontsize=9.5,
        fontweight="bold",
        color="#922b21",
        bbox=dict(boxstyle="round,pad=0.3", fc="#fdedec", ec="#c0392b", alpha=0.9)
    )

    # Configure axes
    ax.set_xscale("log", base=2)
    ax.set_xticks(n)
    
    # Format tick labels nicely
    xtick_labels = []
    for val in n:
        pow2 = int(np.log2(val))
        if val >= 1024:
            s = f"$2^{{{pow2}}}$\n({int(val/1024)}k)"
        else:
            s = f"$2^{{{pow2}}}$\n({int(val)})"
        xtick_labels.append(s)
    ax.set_xticklabels(xtick_labels, fontsize=7.5)

    ax.set_xlabel("Input Size ($N$) [Log2 Scale]", fontsize=11, fontweight="bold")
    ax.set_ylabel("Speedup Ratio ($T_{\\mathrm{recursive}} / T_{\\mathrm{iterative}}$)", fontsize=11, fontweight="bold")
    ax.set_title(
        "Experiment 2: In-Place Speedup Ratio vs. Input Size\nRecursive Radix-2 vs. Iterative Radix-2 FFT ($N = 2^1$ to $2^{18}$)",
        fontsize=12,
        fontweight="bold",
        pad=12
    )

    # Ensure y-axis covers 0 to slightly above max speedup
    y_top = max(9.0, np.max(speedup) + 1.2)
    ax.set_ylim(bottom=0.0, top=y_top)

    ax.grid(True, which="both", ls=":", alpha=0.6)
    ax.legend(loc="upper left", frameon=True, framealpha=0.95, fontsize=10)

    fig.tight_layout()

    # Destination directories
    out_dir = bench_dir / "results"
    out_dir.mkdir(parents=True, exist_ok=True)
    report_dir = bench_dir.parent.parent / "report"

    # Save to bench/results/ (PNG only)
    png_bench = out_dir / "experiment2_speedup.png"
    plt.savefig(png_bench, dpi=300)
    print(f"Saved plot to: {png_bench}")

    # Copy to report/ (PNG only)
    if report_dir.exists():
        png_report = report_dir / "experiment2_speedup.png"
        plt.savefig(png_report, dpi=300)
        print(f"Copied report plot to: {png_report}")

if __name__ == "__main__":
    main()
