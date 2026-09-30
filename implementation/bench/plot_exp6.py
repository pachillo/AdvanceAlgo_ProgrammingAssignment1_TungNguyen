#!/usr/bin/env python3
"""
Experiment 6: Prime vs. Composite Scaling in Mixed-Radix FFT
Plots:
  Panel 1: Execution Time vs. N (Log-Log scale) contrasting Primes vs. Composites
  Panel 2: Speedup Ratio (T_naive / T_mixed) vs. N, demonstrating 1.0x flatline for primes
           and massive O(N log N) speedup for composites.
Outputs PNG only (no PDF).
"""

import sys
import shutil
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def main():
    bench_dir = Path(__file__).resolve().parent
    default_csv = bench_dir / "results" / "experiment6.csv"
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else default_csv

    if not csv_path.exists():
        print(f"Error: CSV file not found at {csv_path}")
        print("Please run the benchmark first (e.g. 'make bench6').")
        sys.exit(1)

    # Load data from CSV using recfromcsv / genfromtxt
    data = np.genfromtxt(csv_path, delimiter=",", names=True, dtype=None, encoding="utf-8")
    
    n_all = data["N"]
    cat_all = data["Category"]
    naive_t = data["Naive_Time_us"]
    mixed_t = data["Mixed_Time_us"]
    speedup = data["Speedup_Ratio"]

    # Filter categories
    mask_prime = (cat_all == "Prime")
    mask_p2 = (cat_all == "PowerOfTwo")
    mask_comp = (cat_all == "CompositeNonPowerOfTwo")

    n_prime = n_all[mask_prime]
    naive_prime = naive_t[mask_prime]
    mixed_prime = mixed_t[mask_prime]
    speedup_prime = speedup[mask_prime]

    n_p2 = n_all[mask_p2]
    naive_p2 = naive_t[mask_p2]
    mixed_p2 = mixed_t[mask_p2]
    speedup_p2 = speedup[mask_p2]

    n_comp = n_all[mask_comp]
    naive_comp = naive_t[mask_comp]
    mixed_comp = mixed_t[mask_comp]
    speedup_comp = speedup[mask_comp]

    print("==========================================================")
    print(" Experiment 6: Prime vs. Composite Scaling Summary        ")
    print("==========================================================")
    print(f"Largest Prime Tested:     N = {n_prime[-1]}")
    print(f"  Naive DFT:              {naive_prime[-1]:.2f} us")
    print(f"  Mixed-Radix FFT:        {mixed_prime[-1]:.2f} us")
    print(f"  Speedup Ratio:          {speedup_prime[-1]:.2f}x (Identical Runtime)")
    print(f"Largest Composite Tested: N = {n_p2[-1]}")
    print(f"  Naive DFT:              {naive_p2[-1]:.2f} us")
    print(f"  Mixed-Radix FFT:        {mixed_p2[-1]:.2f} us")
    print(f"  Speedup Ratio:          {speedup_p2[-1]:.2f}x (Divide-and-Conquer)")
    print("==========================================================\n")

    # Set up 2-panel figure
    plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15.0, 6.5), dpi=300)

    # ----------------------------------------------------
    # PANEL 1: Execution Time vs. N (Log-Log Scale)
    # ----------------------------------------------------
    # Prime inputs: Naive vs Mixed-Radix (Overlapping on O(N^2))
    ax1.plot(n_prime, naive_prime, color="#8e44ad", linestyle="--", linewidth=1.8,
             marker="^", markersize=7.0, label="Naive DFT on Primes", zorder=4)
    ax1.plot(n_prime, mixed_prime, color="#9b59b6", linestyle=":", linewidth=2.2,
             marker="o", markerfacecolor="none", markeredgewidth=2.2, markersize=10.0,
             label="Mixed-Radix FFT on Primes (concentric fallback)", zorder=5)

    # Composite Power-of-2 inputs
    ax1.plot(n_p2, naive_p2, color="#c0392b", linestyle="--", linewidth=1.5, alpha=0.5,
             marker="v", markersize=5.5, label="Naive DFT on Composites", zorder=3)
    ax1.plot(n_p2, mixed_p2, color="#2980b9", linestyle="-", linewidth=2.2,
             marker="s", markersize=6.0, label=r"Mixed-Radix on $2^k$ (Power of 2)", zorder=6)

    # Smooth non-power-of-2 composites
    ax1.plot(n_comp, mixed_comp, color="#27ae60", linestyle="-", linewidth=2.0,
             marker="D", markersize=5.5, label=r"Mixed-Radix on Smooth Composites ($2^a 3^b 5^c$)", zorder=6)

    # Reference Complexity Slopes
    # O(N^2) reference slope anchored at largest prime
    ref_n = np.linspace(n_all.min(), n_all.max(), 100)
    c_n2 = naive_prime[-1] / (n_prime[-1] ** 2)
    ax1.plot(ref_n, c_n2 * (ref_n ** 2), color="#7f8c8d", linestyle="-.", linewidth=1.3, alpha=0.7,
             label=r"Theoretical $\mathcal{O}(N^2)$ Bound")

    # O(N log N) reference slope anchored at largest composite
    c_nlogn = mixed_p2[-1] / (n_p2[-1] * np.log2(n_p2[-1]))
    ax1.plot(ref_n, c_nlogn * (ref_n * np.log2(ref_n)), color="#16a085", linestyle="-.", linewidth=1.3, alpha=0.7,
             label=r"Theoretical $\mathcal{O}(N \log N)$ Bound")

    ax1.set_xscale("log", base=2)
    ax1.set_yscale("log", base=10)
    ax1.set_ylim(0.2, 2.5e7)
    ax1.set_xlabel(r"Input Size $N$ [$\log_2$ scale]", fontsize=11.5, fontweight="bold")
    ax1.set_ylabel(r"Execution Time ($\mu$s) [$\log_{10}$ scale]", fontsize=11.5, fontweight="bold")
    ax1.set_title("Execution Time: Prime Fallback vs. Composite Speedup", fontsize=12.5, fontweight="bold", pad=12)
    ax1.legend(loc="upper left", frameon=True, framealpha=0.92, fontsize=8.8)

    # Annotations on Panel 1
    # Prime fallback annotation at N = 8191
    ax1.annotate(
        f"Mersenne Prime $N = 8191$:\nMixed-Radix falls back to Naive DFT\n$T_{{\\mathrm{{mixed}}}} \\approx T_{{\\mathrm{{naive}}}} = {mixed_prime[-1]:.0f}\\,\\mu\\mathrm{{s}}$",
        xy=(8191, mixed_prime[-1]),
        xytext=(8191 * 0.05, mixed_prime[-1] * 2.2),
        arrowprops=dict(facecolor="#8e44ad", shrink=0.08, width=1.5, headwidth=6),
        fontsize=8.5, fontweight="bold", color="#6c3483",
        bbox=dict(boxstyle="round,pad=0.3", fc="#f4ecf7", ec="#8e44ad", alpha=0.9),
        zorder=7,
    )

    # Composite divergence at N = 8192
    ax1.annotate(
        f"Composite $N = 8192$:\nDivide-and-Conquer Acceleration\n$T_{{\\mathrm{{mixed}}}} = {mixed_p2[-1]:.0f}\\,\\mu\\mathrm{{s}}$",
        xy=(8192, mixed_p2[-1]),
        xytext=(8192 * 0.12, mixed_p2[-1] * 0.15),
        arrowprops=dict(facecolor="#2980b9", shrink=0.08, width=1.5, headwidth=6),
        fontsize=8.5, fontweight="bold", color="#1f618d",
        bbox=dict(boxstyle="round,pad=0.3", fc="#ebf5fb", ec="#2980b9", alpha=0.9),
        zorder=7,
    )

    # ----------------------------------------------------
    # PANEL 2: Speedup Ratio (T_naive / T_mixed) vs. N
    # ----------------------------------------------------
    # Baseline line at 1.0x (No speedup)
    ax2.axhline(1.0, color="#7f8c8d", linestyle=":", linewidth=1.8, label="1.0x Baseline (Identical Performance)", zorder=2)

    # Primes speedup (flatline ~1.0x)
    ax2.plot(n_prime, speedup_prime, color="#8e44ad", linestyle="-", linewidth=2.2,
             marker="o", markersize=6.5, label="Primes (Speedup $\\approx 1.0\\times$)", zorder=5)

    # Composite Powers-of-2 speedup
    ax2.plot(n_p2, speedup_p2, color="#2980b9", linestyle="-", linewidth=2.2,
             marker="s", markersize=6.0, label=r"Power of 2 ($2^k$) Speedup", zorder=6)

    # Smooth non-power-of-2 composites speedup
    ax2.plot(n_comp, speedup_comp, color="#27ae60", linestyle="--", linewidth=2.0,
             marker="D", markersize=5.5, label=r"Smooth Composites ($2^a 3^b 5^c$)", zorder=6)

    ax2.set_xscale("log", base=2)
    ax2.set_yscale("linear")
    ax2.set_ylim(-8.0, 245.0)
    ax2.set_xlabel(r"Input Size $N$ [$\log_2$ scale]", fontsize=11.5, fontweight="bold")
    ax2.set_ylabel(r"Speedup Ratio $\left(\frac{T_{\mathrm{naive}}}{T_{\mathrm{mixed}}}\right)$", fontsize=11.5, fontweight="bold")
    ax2.set_title("Speedup Factor: Flat 1.0x on Primes vs. Massive Gain on Composites", fontsize=12.5, fontweight="bold", pad=12)
    ax2.legend(loc="upper left", frameon=True, framealpha=0.92, fontsize=9.0)

    # Annotation on Panel 2
    # Primes flatline callout
    ax2.annotate(
        "Primes: Speedup = 1.0x\n(Strict O(N^2) Fallback)",
        xy=(n_prime[len(n_prime)//2], speedup_prime[len(speedup_prime)//2]),
        xytext=(n_prime[len(n_prime)//2] * 0.1, 45.0),
        arrowprops=dict(facecolor="#8e44ad", shrink=0.08, width=1.5, headwidth=6),
        fontsize=8.8, fontweight="bold", color="#6c3483",
        bbox=dict(boxstyle="round,pad=0.3", fc="#f4ecf7", ec="#8e44ad", alpha=0.9),
        zorder=7,
    )

    # Max speedup callout at N = 8192
    ax2.annotate(
        f"Composite N = 8192:\nSpeedup = {speedup_p2[-1]:.1f}x",
        xy=(8192, speedup_p2[-1]),
        xytext=(8192 * 0.18, speedup_p2[-1] * 0.88),
        arrowprops=dict(facecolor="#2980b9", shrink=0.08, width=1.5, headwidth=6),
        fontsize=8.8, fontweight="bold", color="#1f618d",
        bbox=dict(boxstyle="round,pad=0.3", fc="#ebf5fb", ec="#2980b9", alpha=0.9),
        zorder=7,
    )

    fig.suptitle("Experiment 6: Prime vs. Composite Scaling in Mixed-Radix FFT\nDirect Empirical Demonstration of Factorization Dependency",
                 fontsize=14.0, fontweight="bold", y=0.98)
    plt.tight_layout(rect=[0, 0, 1, 0.93])

    # Save output
    output_png = bench_dir / "results" / "experiment6_mixed_radix.png"
    output_png.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(output_png, dpi=300, bbox_inches="tight")
    print(f"Saved plot to: {output_png}")

    # Copy to report/
    report_dir = bench_dir.parent.parent / "report"
    if report_dir.exists():
        report_png = report_dir / "experiment6_mixed_radix.png"
        shutil.copyfile(output_png, report_png)
        print(f"Copied report plot to: {report_png}")

if __name__ == "__main__":
    main()
