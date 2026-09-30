#!/usr/bin/env python3
"""
Experiment 5: Numerical Precision Drift
Plots Maximum Absolute Reconstruction Error:
    ||x - IFFT(FFT(x))||_inf
vs. Input Size N on a semi-log scale (Log2 on X, Log10 on Y).
Compares Naive DFT, Recursive FFT, Iterative FFT, and Precomputed Twiddles FFT
against theoretical error growth bounds and machine epsilon.
Outputs PNG only (no PDF).
"""

import sys
import shutil
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def main():
    bench_dir = Path(__file__).resolve().parent
    default_csv = bench_dir / "results" / "experiment5.csv"
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else default_csv

    if not csv_path.exists():
        print(f"Error: CSV file not found at {csv_path}")
        print("Please run the benchmark first (e.g. 'make bench5').")
        sys.exit(1)

    # Load data from CSV
    data = np.genfromtxt(csv_path, delimiter=",", names=True)
    n = data["N"]
    
    naive_linf = data["Naive_max_Linf"]
    rec_linf = data["Recursive_max_Linf"]
    iter_linf = data["Iterative_max_Linf"]
    pre_linf = data["Precomputed_max_Linf"]

    # Filter valid points for Naive DFT (ignoring NaNs)
    naive_valid_mask = ~np.isnan(naive_linf) & (naive_linf > 0)
    n_naive = n[naive_valid_mask]
    naive_linf_valid = naive_linf[naive_valid_mask]

    eps = np.finfo(np.float64).eps  # ~2.220446e-16

    print("==================================================")
    print(" Experiment 5: Numerical Precision Drift Summary  ")
    print("==================================================")
    print(f"Machine Epsilon (double): {eps:.3e}")
    if len(n_naive) > 0:
        print(f"Naive DFT L_inf (N=2):    {naive_linf_valid[0]:.3e}")
        print(f"Naive DFT L_inf (N={int(n_naive[-1])}): {naive_linf_valid[-1]:.3e}")
    print(f"Recursive FFT L_inf (N=2): {rec_linf[0]:.3e}")
    print(f"Recursive FFT L_inf (N={int(n[-1])}): {rec_linf[-1]:.3e}")
    print(f"Iterative FFT L_inf (N=2): {iter_linf[0]:.3e}")
    print(f"Iterative FFT L_inf (N={int(n[-1])}): {iter_linf[-1]:.3e}")
    print(f"Precomputed FFT L_inf (N=2): {pre_linf[0]:.3e}")
    print(f"Precomputed FFT L_inf (N={int(n[-1])}): {pre_linf[-1]:.3e}")
    print("==================================================\n")

    # Set up matplotlib styling
    plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
    fig, ax = plt.subplots(figsize=(11.5, 6.8), dpi=300)

    # 1. Machine epsilon baseline
    ax.axhline(eps, color="#7f8c8d", linestyle=":", linewidth=1.8,
               label=rf"Machine $\varepsilon_{{\mathrm{{mach}}}}$ (IEEE 754 double: $\sim 2.22 \times 10^{{-16}}$)", zorder=2)

    # 2. Theoretical Scaling Curves
    # FFT logarithmic error growth bound: C * eps * log2(N)
    log2_n = np.log2(n)
    c_fft = 2.0
    fft_theory_bound = c_fft * eps * np.sqrt(log2_n)
    ax.plot(n, fft_theory_bound, color="#95a5a6", linestyle="--", linewidth=1.5, alpha=0.85,
            label=rf"FFT Statistical Bound: $\mathcal{{O}}(\varepsilon \sqrt{{\log_2 N}})$", zorder=2)

    # Naive DFT sqrt(N) accumulation bound
    if len(n_naive) > 0:
        c_dft = 1.0 * eps * np.sqrt(n_naive)
        ax.plot(n_naive, c_dft, color="#e67e22", linestyle="-.", linewidth=1.4, alpha=0.8,
                label=rf"Naive DFT Bound: $\mathcal{{O}}(\varepsilon \sqrt{{N}})$", zorder=2)

    # 3. Empirical curves
    if len(n_naive) > 0:
        ax.plot(n_naive, naive_linf_valid, color="#8e44ad", marker="^", markersize=6.5,
                linewidth=2.2, label=r"Naive DFT / IDFT $\|x - \hat{x}\|_\infty$", zorder=5)

    # Note: Recursive and Precomputed Twiddles FFT produce identical mathematical precision.
    # We render Recursive with a solid blue line and large hollow circle markers,
    # and Precomputed with a dashed green line and smaller filled diamonds, so both
    # are clearly distinct and visibly concentric.
    ax.plot(n, rec_linf, color="#1f77b4", linestyle="-", linewidth=2.4,
            marker="o", markerfacecolor="none", markeredgecolor="#1f77b4", markeredgewidth=2.2, markersize=9.0,
            label=r"Recursive Radix-2 FFT (hollow circle $\circ$)", zorder=6)

    ax.plot(n, iter_linf, color="#d35400", marker="s", markersize=5.5,
            linewidth=2.0, label=r"Iterative Radix-2 FFT (recurrence drift)", zorder=4)

    ax.plot(n, pre_linf, color="#27ae60", linestyle="--", linewidth=1.8,
            marker="D", markerfacecolor="#27ae60", markeredgecolor="#1e8449", markersize=4.5,
            label=r"Precomputed Twiddles FFT (dashed $\diamond$, overlaps Recursive)", zorder=7)

    # Format Axes
    ax.set_xscale("log", base=2)
    ax.set_yscale("log", base=10)

    # X-Ticks matching powers of 2
    powers = np.arange(1, 21)
    tick_values = 2 ** powers
    tick_labels = [rf"$2^{{{p}}}$" for p in powers]
    ax.set_xticks(tick_values)
    ax.set_xticklabels(tick_labels, fontsize=9.5)
    ax.set_xlim([1.4, 2**20 * 1.6])

    # Y-limits accommodating all curves comfortably
    ax.set_ylim([3e-17, 3e-10])

    ax.set_xlabel(r"Input Size $N$ [$\log_2$ scale]", fontsize=12, fontweight="bold")
    ax.set_ylabel(r"Maximum Absolute Error $\|x - \mathrm{IFFT}(\mathrm{FFT}(x))\|_\infty$ [$\log_{10}$ scale]",
                  fontsize=12, fontweight="bold")
    ax.set_title("Experiment 5: Numerical Precision Drift\nRound-Trip Reconstruction Error vs. Input Size $N$",
                 fontsize=14, fontweight="bold", pad=15)

    # Shaded band for floating-point tolerance
    ax.axhspan(1e-16, 1e-13, color="#ebf5fb", alpha=0.35, zorder=1)

    # Annotations
    # 1. At N = 8192, highlight Naive DFT vs FFT precision gap
    if len(n_naive) >= 13:
        idx_8192 = np.where(n_naive == 8192)[0]
        if len(idx_8192) > 0:
            i = idx_8192[0]
            val_naive = naive_linf_valid[i]
            val_pre = pre_linf[np.where(n == 8192)[0][0]]
            ax.annotate(
                f"Naive DFT $\\mathcal{{O}}(\\sqrt{{N}})$ Drift\n$\\|e\\|_\\infty = {val_naive:.1e}$ ({val_naive/val_pre:.0f}x error)",
                xy=(8192, val_naive),
                xytext=(8192 * 0.08, val_naive * 3.0),
                arrowprops=dict(facecolor="#8e44ad", shrink=0.08, width=1.5, headwidth=6),
                fontsize=9.0,
                fontweight="bold",
                color="#6c3483",
                bbox=dict(boxstyle="round,pad=0.3", fc="#f4ecf7", ec="#8e44ad", alpha=0.9),
                zorder=8,
            )

    # 2. Iterative FFT recurrence drift annotation
    val_iter_max = iter_linf[-1]
    ax.annotate(
        f"Iterative $W \\leftarrow W \\cdot W_m$ Recurrence Drift\n$\\|e\\|_\\infty = {val_iter_max:.2e}$ (compounded phase error)",
        xy=(2**20, val_iter_max),
        xytext=(2**14 * 0.8, val_iter_max * 1.5),
        arrowprops=dict(facecolor="#d35400", shrink=0.08, width=1.5, headwidth=6),
        fontsize=9.0,
        fontweight="bold",
        color="#a04000",
        bbox=dict(boxstyle="round,pad=0.3", fc="#fef5e7", ec="#d35400", alpha=0.9),
        zorder=8,
    )

    # 3. Precomputed Twiddles & Recursive overlap annotation at N = 2^20
    val_pre_max = pre_linf[-1]
    ax.annotate(
        f"Recursive & Precomputed FFT (Identical Stability)\n$\\|e\\|_\\infty = {val_pre_max:.2e}$ (tracks $\\mathcal{{O}}(\\varepsilon\\sqrt{{\\log_2 N}})$)",
        xy=(2**20, val_pre_max),
        xytext=(2**13 * 0.65, val_pre_max * 0.12),
        arrowprops=dict(facecolor="#1f77b4", shrink=0.08, width=1.5, headwidth=6),
        fontsize=9.0,
        fontweight="bold",
        color="#0e4b75",
        bbox=dict(boxstyle="round,pad=0.3", fc="#ebf5fb", ec="#1f77b4", alpha=0.9),
        zorder=8,
    )

    ax.legend(loc="upper left", frameon=True, framealpha=0.95, facecolor="white", fontsize=9.2)
    plt.tight_layout()

    # Save output
    output_png = bench_dir / "results" / "experiment5_precision.png"
    output_png.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(output_png, dpi=300, bbox_inches="tight")
    print(f"Saved plot to: {output_png}")

    # Copy to report/
    report_dir = bench_dir.parent.parent / "report"
    if report_dir.exists():
        report_png = report_dir / "experiment5_precision.png"
        shutil.copyfile(output_png, report_png)
        print(f"Copied plot to: {report_png}")

if __name__ == "__main__":
    main()
