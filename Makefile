# Makefile for Atom SuperCollider Plugin
# Rebuild command: make rebuild

# Variables
BUILD_DIR := build
SC_PATH := ../supercollider
INSTALL_PREFIX := $(HOME)/Library/Application\ Support/SuperCollider/Extensions
CMAKE_BUILD_TYPE := Release

.PHONY: all configure build install clean rebuild help

# Default target
all: configure build install

# Configure CMake
configure:
	@echo "Configuring CMake..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. \
		-DSC_PATH=$(SC_PATH) \
		-DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX) \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

# Build the plugin
build:
	@echo "Building plugin..."
	@cd $(BUILD_DIR) && cmake --build . --config $(CMAKE_BUILD_TYPE)

# Install the plugin
install: build
	@echo "Installing plugin..."
	@cd $(BUILD_DIR) && cmake --build . --config $(CMAKE_BUILD_TYPE) --target install
	@echo "Plugin installed to $(INSTALL_PREFIX)"

# Clean build directory
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned"

# Rebuild: clean, configure, build, and install
rebuild: clean configure build install
	@echo "Rebuild complete!"

# Help target
help:
	@echo "Available targets:"
	@echo "  make configure  - Configure CMake build system"
	@echo "  make build      - Build the plugin"
	@echo "  make install    - Install the plugin to Extensions directory"
	@echo "  make clean      - Remove build directory"
	@echo "  make rebuild   - Clean, configure, build, and install (recommended)"
	@echo "  make all        - Configure, build, and install"
	@echo ""
	@echo "Variables (can be overridden):"
	@echo "  BUILD_DIR=$(BUILD_DIR)"
	@echo "  SC_PATH=$(SC_PATH)"
	@echo "  INSTALL_PREFIX=$(INSTALL_PREFIX)"
	@echo "  CMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)"
