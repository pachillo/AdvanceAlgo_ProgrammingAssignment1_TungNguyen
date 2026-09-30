BUILD_DIR ?= build
CMAKE ?= cmake
PYTHON ?= python3
VENV_DIR ?= bench/.venv
VENV_PYTHON ?= $(VENV_DIR)/bin/python3
VENV_PIP ?= $(VENV_DIR)/bin/pip

.PHONY: all build run test clean format bench-build bench bench-env bench-plot bench-all bench2-build bench2 bench2-plot bench2-all help

all: build

build:
	@mkdir -p $(BUILD_DIR)
	@$(CMAKE) -B $(BUILD_DIR) -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@$(CMAKE) --build $(BUILD_DIR) -j
	@ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

run: build
	@./$(BUILD_DIR)/fft_app

test: build
	@./$(BUILD_DIR)/fft_test

# Experiment 1 Targets
bench-build: build

bench: bench-build
	@mkdir -p bench/results
	@./$(BUILD_DIR)/fft_bench_exp1 bench/results/experiment1.csv

bench-env:
	@if [ ! -d "$(VENV_DIR)" ]; then \
		echo "Creating Python virtual environment in $(VENV_DIR)..."; \
		$(PYTHON) -m venv $(VENV_DIR); \
	fi
	@echo "Installing dependencies from bench/requirements.txt..."
	@$(VENV_PIP) install --quiet --upgrade pip
	@$(VENV_PIP) install --quiet -r bench/requirements.txt
	@echo "Python virtual environment ready."

bench-plot: bench-env
	@$(VENV_PYTHON) bench/plot_exp1.py bench/results/experiment1.csv

bench-all: bench bench-plot

# Experiment 2 Targets
bench2-build: build

bench2: bench2-build
	@mkdir -p bench/results
	@./$(BUILD_DIR)/fft_bench_exp2 bench/results/experiment2.csv

bench2-plot: bench-env
	@$(VENV_PYTHON) bench/plot_exp2.py bench/results/experiment2.csv

bench2-all: bench2 bench2-plot

# Experiment 3 Targets
bench3-build: build

bench3: bench3-build
	@mkdir -p bench/results
	@./$(BUILD_DIR)/fft_bench_exp3 bench/results/experiment3.csv

bench3-plot: bench-env
	@$(VENV_PYTHON) bench/plot_exp3.py bench/results/experiment3.csv

bench3-all: bench3 bench3-plot

# Experiment 4 Targets
bench4-build: build

bench4: bench4-build
	@mkdir -p bench/results
	@./$(BUILD_DIR)/fft_bench_exp4 bench/results/experiment4.csv

bench4-plot: bench-env
	@$(VENV_PYTHON) bench/plot_exp4.py bench/results/experiment4.csv

bench4-all: bench4 bench4-plot

# Experiment 5 Targets
bench5-build: build

bench5: bench5-build
	@mkdir -p bench/results
	@./$(BUILD_DIR)/fft_bench_exp5 bench/results/experiment5.csv

bench5-plot: bench-env
	@$(VENV_PYTHON) bench/plot_exp5.py bench/results/experiment5.csv

bench5-all: bench5 bench5-plot

# Run All Experiments Target
bench-suite: bench-all bench2-all bench3-all bench4-all bench5-all
	@echo "=========================================="
	@echo " All 5 benchmarks completed successfully! "
	@echo "=========================================="

format:
	@find src include tests bench -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec clang-format -i {} +
	@echo "Code formatting complete."

clean:
	@rm -rf $(BUILD_DIR) compile_commands.json
	@echo "Cleaned build artifacts."

help:
	@echo "Available targets:"
	@echo "  make            - Configure and build all targets"
	@echo "  make run        - Build and run the main application (fft_app)"
	@echo "  make test       - Build and run unit tests (fft_test)"
	@echo "  make bench      - Run Experiment 1 benchmark (Naive vs. Recursive FFT)"
	@echo "  make bench-plot - Generate Experiment 1 plots (log-log)"
	@echo "  make bench-all  - Run Experiment 1 benchmark and plot"
	@echo "  make bench2     - Run Experiment 2 benchmark (Recursive vs. Iterative FFT)"
	@echo "  make bench2-plot- Generate Experiment 2 plot (Speedup ratio vs. N, PNG)"
	@echo "  make bench2-all - Run Experiment 2 benchmark and plot"
	@echo "  make bench3     - Run Experiment 3 benchmark (Iterative vs. Precomputed Twiddles)"
	@echo "  make bench3-plot- Generate Experiment 3 plots (Time & Speedup vs. N, PNG)"
	@echo "  make bench3-all - Run Experiment 3 benchmark and plot"
	@echo "  make bench4     - Run Experiment 4 benchmark (Hardware Cache Cliff)"
	@echo "  make bench4-plot- Generate Experiment 4 plot (Normalized tau(N) vs. Cache, PNG)"
	@echo "  make bench4-all - Run Experiment 4 benchmark and plot"
	@echo "  make bench5     - Run Experiment 5 benchmark (Numerical Precision Drift)"
	@echo "  make bench5-plot- Generate Experiment 5 plot (Reconstruction error vs. N, PNG)"
	@echo "  make bench5-all - Run Experiment 5 benchmark and plot"
	@echo "  make bench-env  - Set up Python virtual environment"
	@echo "  make format     - Format all source files with clang-format"
	@echo "  make clean      - Remove build directory"

