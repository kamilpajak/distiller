#!/bin/bash
set -e

# Install project-specific libraries once if not already installed
# This uses a marker file to avoid redundant operations
MARKER_FILE="/project/.pio/platformio_library_installed"
if [ ! -f "$MARKER_FILE" ]; then
  echo "Installing project libraries (first run only)..."
  cd /project
  pio pkg install -e test
  
  # Create the marker file
  touch "$MARKER_FILE"
else
  echo "Libraries already installed, skipping..."
fi

# Create symbolic link for mock_arduino.h if it doesn't exist and source file exists
if [ ! -f /project/lib/utilities/include/mock_arduino.h ] && [ -f /project/include/test/mock_arduino.h ]; then
  ln -sf /project/include/test/mock_arduino.h /project/lib/utilities/include/mock_arduino.h
fi

# Create symbolic links for DallasTemperature.h and OneWire.h if source locations exist
if [ -d /project/.pio/libdeps/test/DallasTemperature/src ] && [ ! -L /project/include/DallasTemperature/DallasTemperature.h ]; then
  ln -sf /project/.pio/libdeps/test/DallasTemperature/src/* /project/include/DallasTemperature/
fi

if [ -d /project/.pio/libdeps/test/OneWire ] && [ ! -L /project/include/OneWire/OneWire.h ]; then
  ln -sf /project/.pio/libdeps/test/OneWire/*.h /project/include/OneWire/
fi

# Copy Arduino.h from pre-created file in Docker image if it doesn't exist
if [ ! -f /project/include/Arduino.h ]; then
  cp /usr/include/Arduino.h /project/include/Arduino.h
fi

# Create symbolic links in /usr/include for the header files
# Only create links if the target files exist
[ -f /project/include/Arduino.h ] && ln -sf /project/include/Arduino.h /usr/include/Arduino.h

if [ -d /project/include/DallasTemperature ] && [ -f /project/include/DallasTemperature/DallasTemperature.h ]; then
  ln -sf /project/include/DallasTemperature/DallasTemperature.h /usr/include/DallasTemperature/DallasTemperature.h
fi

if [ -d /project/include/OneWire ] && [ -f /project/include/OneWire/OneWire.h ]; then
  ln -sf /project/include/OneWire/OneWire.h /usr/include/OneWire/OneWire.h
fi

# Create symbolic links for the new library structure in /usr/include if the source directories exist
if [ -d /project/lib/hardware_abstractions/include ] && [ "$(ls -A /project/lib/hardware_abstractions/include 2>/dev/null)" ]; then
  ln -sf /project/lib/hardware_abstractions/include/*.h /usr/include/hardware_abstractions/
fi

if [ -d /project/lib/process_controllers/include ] && [ "$(ls -A /project/lib/process_controllers/include 2>/dev/null)" ]; then
  ln -sf /project/lib/process_controllers/include/*.h /usr/include/process_controllers/
fi

if [ -d /project/lib/utilities/include ] && [ "$(ls -A /project/lib/utilities/include 2>/dev/null)" ]; then
  ln -sf /project/lib/utilities/include/*.h /usr/include/utilities/
fi

# Generate compilation database if needed
DB_FILE="/project/compile_commands.json"
if [ ! -f "$DB_FILE" ] || [ "$(find /project -type f -name "*.cpp" -newer "$DB_FILE" 2>/dev/null)" ]; then
  echo "Generating compilation database..."
  pio run --target compiledb
else
  echo "Compilation database up to date, skipping..."
fi

# Available commands
if [ "$1" = "test" ]; then
    # Don't use exec to ensure proper cleanup
    /scripts/run-tests.sh
    # Capture and return the exit code
    RESULT=$?
    # Kill any lingering processes
    killall -q pio 2>/dev/null || true
    exit $RESULT
elif [ "$1" = "format" ]; then
    exec /scripts/run-format.sh
elif [ "$1" = "tidy" ]; then
    exec /scripts/run-tidy.sh
elif [ "$1" = "all" ]; then
    echo "Running format, tidy, and tests sequentially..."
    /scripts/run-format.sh
    /scripts/run-tidy.sh
    /scripts/run-tests.sh
    exit $?
elif [ "$1" = "build" ]; then
    # The second argument is the environment to build
    if [ -z "$2" ]; then
        echo "Error: No environment specified"
        echo "Usage: build <environment>"
        exit 1
    fi
    echo "Building environment: $2"
    exec pio run -e "$2"
elif [ "$1" = "shell" ]; then
    # Launch a shell for interactive use
    exec /bin/bash
else
    echo "Unknown command: $1"
    echo "Available commands: test, format, tidy, all, build, shell"
    exit 1
fi