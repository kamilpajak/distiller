# Distiller Scripts

This directory contains various utility scripts for the Distiller project.

## Primary Development Scripts

### `pio-tools.sh`

The main script for running development tasks. It manages Docker containers and PlatformIO commands.

Features:
- Smart detection of process completion to prevent terminal hanging
- Automatic timeouts and resource cleanup
- Progress monitoring with feedback
- Support for format, tidy, test, and build operations

Usage:
```bash
# Format code
./scripts/pio-tools.sh format

# Run static analysis
./scripts/pio-tools.sh tidy

# Run unit tests
./scripts/pio-tools.sh test

# Build a specific environment
./scripts/pio-tools.sh build prod

# Run all commands
./scripts/pio-tools.sh all
```

### `entrypoint.sh`

The Docker container entrypoint script. It sets up the environment inside the container.

Features:
- Installs libraries if not already installed (using marker files for performance)
- Creates symbolic links for headers
- Generates compilation database for clang tools
- Routes commands to specialized scripts (test, format, tidy)

## Build and Testing Scripts

### `run-tests.sh`

Script for running unit tests. Used by `entrypoint.sh` inside the Docker container.

### `run-format.sh`

Script for formatting code using clang-format. Used by `entrypoint.sh` inside the Docker container.

### `run-tidy.sh`

Script for running static analysis using clang-tidy. Used by `entrypoint.sh` inside the Docker container.

## Utility Scripts

### `generate_compile_commands.py`

A Python script to generate the compilation database for clang tools.

### Other Notes

The `pio-tools.sh` script is designed to work reliably on macOS terminals, which can sometimes experience issues with Docker processes hanging. The script uses smart detection and polling techniques to ensure that Docker processes complete cleanly and don't leave the terminal hanging indefinitely.