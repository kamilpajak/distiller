FROM python:3.9-slim

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    && rm -rf /var/lib/apt/lists/*

# Install PlatformIO
RUN pip install platformio

# Set up working directory
WORKDIR /project

# Copy project files
COPY . .

# Command to run tests
CMD ["pio", "test", "-e", "test"]
