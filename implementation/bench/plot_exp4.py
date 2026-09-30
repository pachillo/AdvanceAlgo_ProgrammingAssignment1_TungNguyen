#!/usr/bin/env python3
"""
Experiment 4: The Hardware "Cache Cliff" Effect
Plots Normalized Execution Time:
    tau(N) = Time(N) / (N * log2(N))
against Input Size N and Working Set Size (N * 16 bytes).
Compares theoretical constant prediction against empirical cache cliffs across L1d, L2, and L3 caches.
Outputs PNG only (no PDF).
"""

import sys
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def main():
    bench_dir = Path(__file__).resolve().parent
    default_csv = bench_dir / "results" / "experiment4.csv"
    csv_path = Path(sys.argv[1]) if len(sys.argv) > 1 else default_csv

    if not csv_path.exists():
        print(f"Error: CSV file not found at {csv_path}")
        print("Please run the benchmark first (e.g. 'make bench4').")
        sys.exit(1)

    # Load data
    data = np.genfromtxt(csv_path, delimiter=",", names=True)
    n = data["N"]
    working_set_kb = data["Working_Set_KB"]
    tau_iter = data["Tau_Iterative_ns"]
    tau_pre = data["Tau_Precomputed_ns"]

    # Filter for N >= 32 to visualize steady-state asymptotic regime without small-N initialization overhead
    mask = n >= 32
    n_steady = n[mask]
    working_set_steady = working_set_kb[mask]
    tau_iter_steady = tau_iter[mask]
    tau_pre_steady = tau_pre[mask]

    # Baseline theoretical constant (median over L1-fitting sizes)
    tau_iter_baseline = np.median(tau_iter[(n >= 64) & (n <= 2048)])
    tau_pre_baseline = np.median(tau_pre[(n >= 64) & (n <= 2048)])

    print("==================================================")
    print(" Experiment 4: Hardware Cache Cliff Summary       ")
    print("==================================================")
    print(f"Iterative Baseline tau (L1 fit):  {tau_iter_baseline:.2f} ns/element")
    print(f"Iterative L3/RAM tau (N = 1M):    {tau_iter[-1]:.2f} ns/element (+{(tau_iter[-1]/tau_iter_baseline - 1)*100:.1f}%)")
    print(f"Precomputed Baseline tau (L1 fit):{tau_pre_baseline:.2f} ns/element")
    print(f"Precomputed L3/RAM tau (N = 1M):  {tau_pre[-1]:.2f} ns/element (+{(tau_pre[-1]/tau_pre_baseline - 1)*100:.1f}%)")
    print("==================================================\n")

    # Set up matplotlib styling
    plt.style.use("seaborn-v0_8-whitegrid" if "seaborn-v0_8-whitegrid" in plt.style.available else "default")
    fig, ax = plt.subplots(figsize=(10.5, 6.0), dpi=300)

    # Cache boundary background regions
    # L1d cache: ~32-48 KB (N = 2048 to 4096)
    ax.axvspan(2048, 4096, color="#d4efdf", alpha=0.5, label="L1d Cache Boundary (~32–64 KB)")
    # L2 cache: ~1-2 MB (N = 65536 to 131072)
    ax.axvspan(65536, 131072, color="#fcf3cf", alpha=0.5, label="L2 Cache Boundary (~1–2 MB)")
    # L3 cache: ~12-16 MB (N = 524288 to 1048576)
    ax.axvspan(524288, 1048576, color="#fadbd8", alpha=0.5, label="L3 Cache Boundary (~8–16 MB)")

    # Theoretical flat lines
    ax.axhline(
        tau_iter_baseline, color="#c0392b", linestyle="--", linewidth=1.2, alpha=0.6,
        label=f"Theory: Flat Horizontal Line ($\\tau_{{iter}} = {tau_iter_baseline:.2f}$ ns)"
    )
    ax.axhline(
        tau_pre_baseline, color="#2980b9", linestyle="--", linewidth=1.2, alpha=0.6,
        label=f"Theory: Flat Horizontal Line ($\\tau_{{pre}} = {tau_pre_baseline:.2f}$ ns)"
    )

    # Empirical curves
    ax.plot(
        n_steady, tau_iter_steady,
        marker="o", markersize=7, linewidth=2.3, color="#c0392b",
        label="Empirical Iterative Radix-2 $\\tau(N)$"
    )
    ax.plot(
        n_steady, tau_pre_steady,
        marker="s", markersize=7, linewidth=2.3, color="#2980b9",
        label="Empirical Precomputed Twiddles $\\tau(N)$"
    )

    # Annotate cache cliffs
    ax.annotate(
        f"L1 Exceeded\n$\\tau \\approx {tau_iter[n == 4096][0]:.2f}$ ns",
        xy=(4096, tau_iter[n == 4096][0]),
        xytext=(4096 * 0.35, tau_iter[n == 4096][0] + 1.2),
        arrowprops=dict(facecolor="#27ae60", shrink=0.08, width=1.3, headwidth=5),
        fontsize=8.5, fontweight="bold", color="#1e8449",
        bbox=dict(boxstyle="round,pad=0.25", fc="#eafaf1", ec="#27ae60", alpha=0.9)
    )

    ax.annotate(
        f"L2 Cliff\n$\\tau \\approx {tau_iter[n == 262144][0]:.2f}$ ns",
        xy=(262144, tau_iter[n == 262144][0]),
        xytext=(262144 * 0.35, tau_iter[n == 262144][0] + 1.2),
        arrowprops=dict(facecolor="#d4ac0d", shrink=0.08, width=1.3, headwidth=5),
        fontsize=8.5, fontweight="bold", color="#7d6608",
        bbox=dict(boxstyle="round,pad=0.25", fc="#fef9e7", ec="#d4ac0d", alpha=0.9)
    )

    ax.annotate(
        f"L3 Cache Cliff\n$\\tau = {tau_iter[-1]:.2f}$ ns (+50%)",
        xy=(1048576, tau_iter[-1]),
        xytext=(1048576 * 0.28, tau_iter[-1] + 1.3),
        arrowprops=dict(facecolor="#c0392b", shrink=0.08, width=1.3, headwidth=5),
        fontsize=8.5, fontweight="bold", color="#922b21",
        bbox=dict(boxstyle="round,pad=0.25", fc="#fdedec", ec="#c0392b", alpha=0.9)
    )

    # Axes configuration
    ax.set_xscale("log", base=2)
    ax.set_xticks(n_steady)

    # Format x-tick labels with both N and Working Set Size in KB/MB
    xtick_labels = []
    for val in n_steady:
        pow2 = int(np.log2(val))
        kb = val * 16 / 1024
        if kb >= 1024:
            size_str = f"{int(kb/1024)}MB"
        else:
            size_str = f"{int(kb)}KB"
        xtick_labels.append(f"$2^{{{pow2}}}$\n({size_str})")

    ax.set_xticklabels(xtick_labels, fontsize=8)
    ax.set_xlabel("Input Size $N$ and Working Set Memory Footprint ($N \\times 16$ bytes) [Log2 Scale]",
                  fontsize=11, fontweight="bold")
    ax.set_ylabel("Normalized Time per Element: $\\tau(N) = \\frac{\\mathrm{Time}(N)}{N \\log_2 N}$ (ns)",
                  fontsize=11, fontweight="bold")
    ax.set_title(
        "Experiment 4: The Hardware \"Cache Cliff\" Effect\n"
        "Normalized Time per Element vs. CPU Cache Memory Hierarchy (L1d, L2, L3)",
        fontsize=12, fontweight="bold", pad=12
    )

    ax.set_ylim(bottom=1.5, top=9.5)
    ax.grid(True, which="both", ls=":", alpha=0.6)
    ax.legend(loc="upper left", frameon=True, framealpha=0.95, fontsize=8.5)

    fig.tight_layout()

    # Destination paths
    out_dir = bench_dir / "results"
    out_dir.mkdir(parents=True, exist_ok=True)
    report_dir = bench_dir.parent.parent / "report"

    png_bench = out_dir / "experiment4_cache_cliff.png"
    plt.savefig(png_bench, dpi=300)
    print(f"Saved plot to: {png_bench}")

    if report_dir.exists():
        png_report = report_dir / "experiment4_cache_cliff.png"
        plt.savefig(png_report, dpi=300)
        print(f"Copied report plot to: {png_report}")

if __name__ == "__main__":
    main()
