#!/bin/bash
set -e

# Create symbolic link for mock_arduino.h if it doesn't exist
if [ ! -f /project/src/mock_arduino.h ]; then
  ln -sf /project/test/mock_arduino.h /project/src/mock_arduino.h
fi

# Install libraries first
cd /project
pio pkg install -e test

# Create symbolic links for DallasTemperature.h and OneWire.h if they don't exist
if [ ! -d /project/include/DallasTemperature ]; then
  mkdir -p /project/include/DallasTemperature
  if [ -d /project/.pio/libdeps/test/DallasTemperature/src ]; then
    ln -sf /project/.pio/libdeps/test/DallasTemperature/src/* /project/include/DallasTemperature/
  fi
fi

if [ ! -d /project/include/OneWire ]; then
  mkdir -p /project/include/OneWire
  if [ -d /project/.pio/libdeps/test/OneWire ]; then
    ln -sf /project/.pio/libdeps/test/OneWire/*.h /project/include/OneWire/
  fi
fi

# Create a mock Arduino.h file if it doesn't exist
if [ ! -f /project/include/Arduino.h ]; then
  mkdir -p /project/include
  echo "#ifndef ARDUINO_H" > /project/include/Arduino.h
  echo "#define ARDUINO_H" >> /project/include/Arduino.h
  echo "#ifdef UNIT_TEST" >> /project/include/Arduino.h
  echo "#include \"mock_arduino.h\"" >> /project/include/Arduino.h
  echo "#else" >> /project/include/Arduino.h
  echo "// Minimal Arduino.h for non-test builds" >> /project/include/Arduino.h
  echo "#include <cstdint>" >> /project/include/Arduino.h
  echo "" >> /project/include/Arduino.h
  echo "// Function declarations" >> /project/include/Arduino.h
  echo "unsigned long millis();" >> /project/include/Arduino.h
  echo "" >> /project/include/Arduino.h
  echo "// Constants" >> /project/include/Arduino.h
  echo "const int OUTPUT = 1;" >> /project/include/Arduino.h
  echo "const int HIGH = 1;" >> /project/include/Arduino.h
  echo "const int LOW = 0;" >> /project/include/Arduino.h
  echo "" >> /project/include/Arduino.h
  echo "// Function declarations" >> /project/include/Arduino.h
  echo "void pinMode(int pin, int mode);" >> /project/include/Arduino.h
  echo "void digitalWrite(int pin, int value);" >> /project/include/Arduino.h
  echo "void delay(unsigned long ms);" >> /project/include/Arduino.h
  echo "#endif" >> /project/include/Arduino.h
  echo "" >> /project/include/Arduino.h
  echo "#endif // ARDUINO_H" >> /project/include/Arduino.h
fi

# Create symbolic links in /usr/include for the header files
ln -sf /project/include/Arduino.h /usr/include/Arduino.h
mkdir -p /usr/include/DallasTemperature
ln -sf /project/include/DallasTemperature/DallasTemperature.h /usr/include/DallasTemperature/DallasTemperature.h
mkdir -p /usr/include/OneWire
ln -sf /project/include/OneWire/OneWire.h /usr/include/OneWire/OneWire.h

# Generate compilation database
echo "Generating compilation database..."
pio run --target compiledb

# Available commands
if [ "$1" = "test" ]; then
    exec /scripts/run-tests.sh
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
else
    echo "Unknown command: $1"
    echo "Available commands: test, format, tidy, all"
    exit 1
fi
