# Optimized Dockerfile for Distiller project
FROM ubuntu:24.04

# Install basic dependencies in a single layer with minimal installation
# and cleanup in the same step to reduce layer size
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    git \
    python3 \
    python3-pip \
    python3.12-venv \
    wget \
    gnupg \
    lsb-release \
    software-properties-common \
    cmake \
    libgtest-dev \
    libgmock-dev \
    && rm -rf /var/lib/apt/lists/*

# Install latest clang tools from LLVM repository and create symlinks
# in a single layer to optimize cache usage
RUN wget -q https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 21 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    clang-21 \
    clang-format-21 \
    clang-tidy-21 \
    && rm -rf /var/lib/apt/lists/* && \
    rm llvm.sh && \
    ln -s /usr/bin/clang-format-21 /usr/bin/clang-format && \
    ln -s /usr/bin/clang-tidy-21 /usr/bin/clang-tidy

# Install PlatformIO in a virtual environment
# Use --no-cache-dir to avoid storing pip cache in the image
RUN python3 -m venv /opt/platformio && \
    /opt/platformio/bin/pip install --no-cache-dir platformio && \
    ln -s /opt/platformio/bin/platformio /usr/local/bin/platformio && \
    ln -s /opt/platformio/bin/pio /usr/local/bin/pio

# Pre-install common PlatformIO platforms and packages
# This improves rebuild speed as these won't need to be downloaded each time
RUN mkdir -p /root/.platformio && \
    platformio platform install native && \
    platformio platform install atmelsam && \
    platformio lib --global install \
    "google/googletest@1.15.2" \
    "bogde/HX711@^0.7.5" \
    "milesburton/DallasTemperature@^3.11.0" \
    "br3ttb/PID@^1.2.1" \
    "duinowitchery/hd44780@^1.3.2" \
    "tcmenu/TaskManagerIO@^1.4.3" \
    "paulstoffregen/OneWire@^2.3.7" \
    "arduino-libraries/SD@^1.2.4"

# Set up working directory
WORKDIR /project

# Copy all scripts in a single layer
COPY scripts/run-tests.sh \
     scripts/run-format.sh \
     scripts/run-tidy.sh \
     scripts/entrypoint.sh /scripts/

# Make scripts executable
RUN chmod +x /scripts/*.sh

# Create directory structure for symlinks
# This prevents entrypoint.sh from having to create these directories on each run
RUN mkdir -p /project/include/DallasTemperature \
    /project/include/OneWire \
    /project/lib/hardware_abstractions/include \
    /project/lib/process_controllers/include \
    /project/lib/utilities/include \
    /usr/include/DallasTemperature \
    /usr/include/OneWire \
    /usr/include/hardware_abstractions \
    /usr/include/process_controllers \
    /usr/include/utilities

# Create a mock Arduino.h file to avoid creating it on each run
RUN echo "#ifndef ARDUINO_H" > /usr/include/Arduino.h && \
    echo "#define ARDUINO_H" >> /usr/include/Arduino.h && \
    echo "#ifdef UNIT_TEST" >> /usr/include/Arduino.h && \
    echo "#include \"mock_arduino.h\"" >> /usr/include/Arduino.h && \
    echo "#else" >> /usr/include/Arduino.h && \
    echo "// Minimal Arduino.h for non-test builds" >> /usr/include/Arduino.h && \
    echo "#include <cstdint>" >> /usr/include/Arduino.h && \
    echo "" >> /usr/include/Arduino.h && \
    echo "// Function declarations" >> /usr/include/Arduino.h && \
    echo "unsigned long millis();" >> /usr/include/Arduino.h && \
    echo "" >> /usr/include/Arduino.h && \
    echo "// Constants" >> /usr/include/Arduino.h && \
    echo "const int OUTPUT = 1;" >> /usr/include/Arduino.h && \
    echo "const int HIGH = 1;" >> /usr/include/Arduino.h && \
    echo "const int LOW = 0;" >> /usr/include/Arduino.h && \
    echo "" >> /usr/include/Arduino.h && \
    echo "// Function declarations" >> /usr/include/Arduino.h && \
    echo "void pinMode(int pin, int mode);" >> /usr/include/Arduino.h && \
    echo "void digitalWrite(int pin, int value);" >> /usr/include/Arduino.h && \
    echo "void delay(unsigned long ms);" >> /usr/include/Arduino.h && \
    echo "#endif" >> /usr/include/Arduino.h && \
    echo "" >> /usr/include/Arduino.h && \
    echo "#endif // ARDUINO_H" >> /usr/include/Arduino.h

# Set entrypoint
ENTRYPOINT ["/scripts/entrypoint.sh"]

# Default command (can be overridden)
CMD ["test"]