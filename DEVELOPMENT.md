# Development Guide

This document outlines the development workflow and tools used in the Distiller project.

## Development Tools

The project uses PlatformIO for build management, testing, code formatting, and static analysis.

### Development Tools in Docker

The project uses a Docker container (`distiller-tools`) for development tasks. This ensures a consistent environment across different machines and simplifies the setup process.

#### Docker Container Commands

The Docker container supports the following commands:

- **format**: Format code using clang-format
- **tidy**: Run static analysis using clang-tidy
- **test**: Run unit tests
- **all**: Run all commands (format, tidy, test)

#### Convenience Script

A convenience script `scripts/pio-tools.sh` is provided to make it easier to run the Docker container with the appropriate commands:

```bash
# Format code
./scripts/pio-tools.sh format

# Run static analysis
./scripts/pio-tools.sh tidy

# Run unit tests
./scripts/pio-tools.sh test

# Run all commands (format, tidy, test)
./scripts/pio-tools.sh all

# Show help
./scripts/pio-tools.sh help
```

This script:
1. Checks if Docker is installed
2. Builds the Docker image if it doesn't exist
3. Runs the Docker container with the specified command
4. Mounts the current directory to `/project` in the container

Under the hood, the Docker container uses PlatformIO CLI commands to perform these tasks:
- Code formatting uses `clang-format` with the configuration in `.clang-format`
- Static analysis uses `clang-tidy` with the configuration in `.clang-tidy`
- Unit testing uses Google Test framework with PlatformIO's test runner

### Configuration

The PlatformIO configuration is in `platformio.ini`. The following settings have been added:

```ini
[platformio]
extra_configs = 
    scripts/generate_compile_commands.py
packages = 
    tool-clang-format @ 15.0.7

[env:mkrwifi1010]
# ... existing configuration ...
check_tool = clangtidy
check_flags = 
    clangtidy: --config-file=.clang-tidy
check_patterns = 
    src
    include
    test
```

These settings configure the clang-format and clang-tidy tools for use with PlatformIO.

### Static Analysis with Clang-Tidy

The project uses clang-tidy for static analysis with a configuration in `.clang-tidy`. The following checks are enabled:

- `clang-diagnostic-*`: Compiler diagnostics
- `clang-analyzer-*`: Static analyzer checks
- `cppcoreguidelines-*`: C++ Core Guidelines checks
- `modernize-*`: Modernization checks
- `performance-*`: Performance checks
- `readability-*`: Readability checks

Some checks are disabled or configured to reduce noise:
- `-modernize-use-trailing-return-type`: Disabled because it's a stylistic choice
- `-readability-identifier-length`: Disabled to allow short variable names in certain contexts

#### Special Configurations

1. **Test Fixtures**: 
   - Protected member variables in test fixtures are allowed (common in Google Test)
   - The `cppcoreguidelines-non-private-member-variables-in-classes` check is disabled globally
   - This is a deliberate choice as Google Test fixtures commonly use protected member variables

2. **Magic Numbers**:
   - Common values (0, 1, 2, -1) are excluded from magic number warnings
   - Other magic numbers should be replaced with named constants

3. **Third-Party Code**:
   - Google Test/Mock headers are excluded from analysis using `HeaderFilterRegex`
   - This prevents warnings in code we don't control

4. **Modernize Type Traits**:
   - The `modernize-type-traits` check is disabled globally
   - This is because it conflicts with Google Test's implementation of MOCK_METHOD macros

#### NOLINT Comments

In some cases, NOLINT comments are used to suppress specific warnings:

```cpp
// Suppress a specific warning
int x = 42; // NOLINT(cppcoreguidelines-avoid-magic-numbers)

// Suppress multiple warnings
void foo() { // NOLINT(cppcoreguidelines-pro-type-member-init,cppcoreguidelines-special-member-functions)
  // ...
}
```

Use NOLINT comments sparingly and only when:
1. The warning is a false positive
2. Fixing the warning would make the code less readable
3. The warning is in test code where different standards apply

## Development Workflow

1. Write code in the `src` directory
2. Write tests in the `test` directory
3. Run tests with `./scripts/pio-tools.sh test`
4. Format code with `./scripts/pio-tools.sh format`
5. Run static analysis with `./scripts/pio-tools.sh tidy`
6. Run all checks with `./scripts/pio-tools.sh all`
7. Build and upload to the Arduino MKR WiFi 1010 using PlatformIO IDE or CLI
8. Monitor the serial output using PlatformIO IDE or CLI

The Docker container handles the development environment setup, including:
- Installing all required dependencies
- Setting up the compilation database
- Creating necessary symbolic links
- Configuring the build environment

This ensures a consistent development experience across different machines and simplifies the setup process.
