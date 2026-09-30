#!/usr/bin/env python3
"""
Experiment 1: Asymptotic Scaling & Crossover Point Plotter
Reads benchmark CSV data, computes log-log empirical slopes (O(N^2) vs O(N log N)),
detects the crossover point, and generates publication-grade PDF and PNG plots.
"""

import sys
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def main():
    bench_dir = Path(__file__).resolve().parent
    default_csv = bench_dir / "results" / "experiment1.csv"
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else default_csv

    if not csv_path.exists():
        print(f"Error: CSV file not found at {csv_path}")
        print("Please run the benchmark first (e.g. 'make bench').")
        sys.exit(1)

    # Load data
    data = np.genfromtxt(csv_path, delimiter=",", names=True)
    n = data["N"]
    dft_time_ns = data["DFT_mean_ns"]
    fft_time_ns = data["FFT_mean_ns"]

    # Convert to microseconds for cleaner display
    dft_time_us = dft_time_ns / 1000.0
    fft_time_us = fft_time_ns / 1000.0

    # Fit empirical slopes in log-log space (using N >= 16 where asymptotic regime dominates)
    fit_mask = n >= 16
    log2_n_fit = np.log2(n[fit_mask])
    log2_dft_fit = np.log2(dft_time_us[fit_mask])
    log2_fft_fit = np.log2(fft_time_us[fit_mask])

    slope_dft, intercept_dft = np.polyfit(log2_n_fit, log2_dft_fit, 1)
    slope_fft, intercept_fft = np.polyfit(log2_n_fit, log2_fft_fit, 1)

    print("==================================================")
    print(" Experiment 1: Empirical Slope Analysis           ")
    print("==================================================")
    print(f"Naive DFT measured slope (k in N^k):     {slope_dft:.3f} (Hypothesis: ~2.00)")
    print(f"Cooley-Tukey FFT measured slope (k):    {slope_fft:.3f} (Hypothesis: ~1.05)")

    # Find crossover point where FFT becomes faster than DFT
    crossover_idx = np.where(fft_time_us <= dft_time_us)[0]
    if len(crossover_idx) > 0:
        crossover_n = int(n[crossover_idx[0]])
        print(f"Observed Crossover Point:               N = {crossover_n}")
    else:
        crossover_n = None
        print("No crossover observed in input range.")
    print("==================================================\n")

    # Set up matplotlib plot styling
    plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
    fig, ax = plt.subplots(figsize=(8.5, 5.5), dpi=300)

    # Plot empirical data points and curves
    ax.plot(
        n, dft_time_us,
        marker="o", markersize=6, linewidth=2, color="#1f77b4",
        label=f"Naive DFT (Empirical slope $k = {slope_dft:.2f}$)"
    )
    ax.plot(
        n, fft_time_us,
        marker="s", markersize=6, linewidth=2, color="#d62728",
        label=f"Cooley-Tukey FFT (Empirical slope $k = {slope_fft:.2f}$)"
    )

    # Plot theoretical fitted lines over asymptotic range
    n_fit_range = np.array([16, 8192])
    fit_dft_vals = 2 ** (slope_dft * np.log2(n_fit_range) + intercept_dft)
    fit_fft_vals = 2 ** (slope_fft * np.log2(n_fit_range) + intercept_fft)

    ax.plot(n_fit_range, fit_dft_vals, "--", color="#1f77b4", alpha=0.5, label="DFT Asymptotic Fit ($O(N^2)$)")
    ax.plot(n_fit_range, fit_fft_vals, "--", color="#d62728", alpha=0.5, label="FFT Asymptotic Fit ($O(N \\log N)$)")

    # Highlight Crossover point
    if crossover_n is not None:
        c_y = fft_time_us[crossover_idx[0]]
        ax.scatter([crossover_n], [c_y], color="#2ca02c", s=130, zorder=5)
        ax.annotate(
            f"Crossover\n$N = {crossover_n}$",
            xy=(crossover_n, c_y),
            xytext=(crossover_n * 2.2, c_y * 0.3),
            arrowprops=dict(facecolor="#2ca02c", shrink=0.08, width=1.5, headwidth=7),
            fontsize=10,
            fontweight="bold",
            color="#1b5e20",
            bbox=dict(boxstyle="round,pad=0.3", fc="#e8f5e9", ec="#2ca02c", alpha=0.9)
        )

    # Log-Log axes configuration
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    ax.set_xticks(n)
    ax.set_xticklabels([f"$2^{{{int(np.log2(val))}}}$\n({int(val)})" for val in n], fontsize=8)

    ax.set_xlabel("Input Size ($N$)", fontsize=11, fontweight="bold")
    ax.set_ylabel("Execution Time ($\\mu$s) [Log Scale]", fontsize=11, fontweight="bold")
    ax.set_title("Experiment 1: Execution Time vs. Input Size (Log-Log Scale)\nNaive DFT vs. Recursive Cooley-Tukey FFT", fontsize=12, fontweight="bold", pad=12)

    ax.grid(True, which="both", ls=":", alpha=0.6)
    ax.legend(loc="upper left", frameon=True, framealpha=0.9)

    fig.tight_layout()

    # Destination directories
    out_dir = bench_dir / "results"
    out_dir.mkdir(parents=True, exist_ok=True)
    report_dir = bench_dir.parent.parent / "report"

    # Save to bench/results/
    png_bench = out_dir / "experiment1_loglog.png"
    plt.savefig(png_bench, dpi=300)
    print(f"Saved plot to: {png_bench}")

    # Also save directly into report/ if report directory exists
    if report_dir.exists():
        png_report = report_dir / "experiment1_loglog.png"
        plt.savefig(png_report, dpi=300)

if __name__ == "__main__":
    main()
