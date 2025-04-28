#!/bin/bash
set -e

# Script to run development tools in the Docker container for the Distiller project

function show_help {
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  format    - Format code using clang-format"
    echo "  tidy      - Run static analysis using clang-tidy"
    echo "  test      - Run unit tests"
    echo "  all       - Run all commands (format, tidy, test)"
    echo "  help      - Show this help message"
    echo ""
    echo "Example: $0 format"
}

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed or not in PATH"
    exit 1
fi

# Build the Docker image if it doesn't exist
if ! docker images | grep -q distiller-tools; then
    echo "Building Docker image..."
    docker build -t distiller-tools .
fi

case "$1" in
    format)
        echo "Running code formatting in Docker container..."
        docker run --rm -v "$(pwd):/project" distiller-tools format
        echo "Formatting complete!"
        ;;
    tidy)
        echo "Running static analysis in Docker container..."
        docker run --rm -v "$(pwd):/project" distiller-tools tidy
        echo "Static analysis complete!"
        ;;
    test)
        echo "Running unit tests in Docker container..."
        docker run --rm -v "$(pwd):/project" distiller-tools test
        echo "Tests complete!"
        ;;
    all)
        echo "Running all commands in Docker container..."
        docker run --rm -v "$(pwd):/project" distiller-tools all
        echo "All commands complete!"
        ;;
    help|*)
        show_help
        ;;
esac
