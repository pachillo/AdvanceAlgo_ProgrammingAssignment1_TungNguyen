BUILD_DIR ?= build
CMAKE ?= cmake

.PHONY: all build run test clean format help

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

format:
	@find src include tests -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec clang-format -i {} +
	@echo "Code formatting complete."

clean:
	@rm -rf $(BUILD_DIR) compile_commands.json
	@echo "Cleaned build artifacts."

help:
	@echo "Available targets:"
	@echo "  make         - Configure and build all targets"
	@echo "  make run     - Build and run the main application (fft_app)"
	@echo "  make test    - Build and run unit tests (fft_test)"
	@echo "  make format  - Format all source files with clang-format"
	@echo "  make clean   - Remove build directory"
