#!/bin/bash
set -e

# Script to run development tools in the Docker container for the Distiller project

function show_help {
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  format               - Format code using clang-format"
    echo "  tidy                 - Run static analysis using clang-tidy"
    echo "  test                 - Run unit tests"
    echo "  build <environment>  - Build specific environment (e.g., prod_debug)"
    echo "  shell                - Open interactive shell in container"
    echo "  all                  - Run all commands (format, tidy, test)"
    echo "  help                 - Show this help message"
    echo ""
    echo "Example: $0 format"
    echo "Example: $0 build prod_debug"
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
    build)
        if [ -z "$2" ]; then
            echo "Error: No environment specified"
            echo "Usage: $0 build <environment>"
            echo "Example: $0 build prod_debug"
            exit 1
        fi
        echo "Building environment: $2 in Docker container..."
        docker run --rm -v "$(pwd):/project" distiller-tools build "$2"
        echo "Build complete!"
        ;;
    shell)
        echo "Starting interactive shell in Docker container..."
        docker run --rm -it -v "$(pwd):/project" distiller-tools shell
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
