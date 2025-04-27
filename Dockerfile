FROM ubuntu:24.04

# Install basic dependencies
RUN apt-get update && apt-get install -y \
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

# Install latest clang tools from LLVM repository
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 21 && \
    apt-get update && apt-get install -y \
    clang-21 \
    clang-format-21 \
    clang-tidy-21 \
    && rm -rf /var/lib/apt/lists/* && \
    rm llvm.sh

# Create symlinks for clang tools
RUN ln -s /usr/bin/clang-format-21 /usr/bin/clang-format && \
    ln -s /usr/bin/clang-tidy-21 /usr/bin/clang-tidy

# Install PlatformIO in a virtual environment
RUN python3 -m venv /opt/platformio && \
    /opt/platformio/bin/pip install platformio && \
    ln -s /opt/platformio/bin/platformio /usr/local/bin/platformio && \
    ln -s /opt/platformio/bin/pio /usr/local/bin/pio

# Set up working directory
WORKDIR /project

# Create directory for scripts
RUN mkdir -p /scripts

# Add helper scripts
COPY scripts/run-tests.sh /scripts/
COPY scripts/run-format.sh /scripts/
COPY scripts/run-tidy.sh /scripts/
COPY scripts/entrypoint.sh /scripts/

# Make scripts executable
RUN chmod +x /scripts/*.sh

# Set entrypoint
ENTRYPOINT ["/scripts/entrypoint.sh"]

# Default command (can be overridden)
CMD ["test"]
