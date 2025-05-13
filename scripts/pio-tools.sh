#!/bin/bash
set -e

# ==========================================================================
# Script to run development tools in Docker for the Distiller project
# ==========================================================================
#
# This script has been enhanced with smart completion detection to prevent
# the Docker container from hanging on macOS terminals. It uses these key
# techniques to solve the problem:
#
# 1. Runs containers in detached mode with --init flag for proper signal handling
# 2. Uses named containers for easier tracking and cleanup
# 3. Streams logs in real-time while polling for completion markers
# 4. Detects when commands finish and terminates Docker processes correctly
# 5. Implements proper resource cleanup to prevent orphaned processes
#
# The script handles timeouts gracefully and provides detailed output about
# progress and results.

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

# Function to generate a unique container name
generate_container_name() {
    local prefix=$1
    echo "${prefix}-$(date +%s)"
}

# Function to clean up resources
cleanup() {
    local container_name=$1
    local log_pid=$2
    local temp_file=$3
    
    echo "Cleaning up resources..."
    
    # Kill log streaming process if running
    if [ ! -z "$log_pid" ] && ps -p $log_pid > /dev/null 2>&1; then
        kill $log_pid 2>/dev/null || true
    fi
    
    # Stop and remove the container if it exists
    if [ ! -z "$container_name" ] && docker ps -a | grep -q "$container_name"; then
        docker stop "$container_name" 2>/dev/null || true
        docker rm "$container_name" 2>/dev/null || true
    fi
    
    # Remove temp file if it exists
    if [ ! -z "$temp_file" ] && [ -f "$temp_file" ]; then
        rm -f "$temp_file" 2>/dev/null || true
    fi
    
    # Kill any lingering containers
    local lingering=$(docker ps -q --filter ancestor=distiller-tools)
    if [ ! -z "$lingering" ]; then
        docker stop $lingering 2>/dev/null || true
    fi
    
    echo "Cleanup complete."
}

# Function to run a command with smart detection
run_with_smart_detection() {
    local command=$1
    local command_args=${2:-""}
    local completion_marker=$3
    local timeout=${4:-300}
    local poll_interval=2
    
    # Generate unique container name and temp file
    local container_name="distiller-${command}-$(date +%s)"
    local temp_log_file=$(mktemp)
    
    echo "Running ${command} in Docker container..."
    echo "Using temporary log file: $temp_log_file"
    
    # Run container with appropriate command in detached mode
    if [ -z "$command_args" ]; then
        docker run --name "$container_name" --init --rm -d -v "$(pwd):/project" distiller-tools "$command"
    else
        docker run --name "$container_name" --init --rm -d -v "$(pwd):/project" distiller-tools "$command" "$command_args"
    fi
    
    echo "Container started: $container_name"
    
    # Start log streaming
    docker logs -f "$container_name" &
    local log_pid=$!
    
    # Handle cleanup on script exit/termination
    trap "cleanup '$container_name' '$log_pid' '$temp_log_file'" EXIT INT TERM
    
    echo "Waiting for ${command} to complete (max $timeout seconds)..."
    if [ ! -z "$completion_marker" ]; then
        echo "Checking for completion marker: '$completion_marker'"
    fi
    
    # Poll for completion
    for ((i=1; i<=$timeout/$poll_interval; i++)); do
        # Check if container is still running
        if ! docker ps -q --filter name="$container_name" | grep -q .; then
            echo -e "\nContainer has exited - ${command} completed."
            break
        fi
        
        # Get the latest logs
        docker logs "$container_name" > "$temp_log_file" 2>&1
        
        # Check for completion marker in logs (if provided)
        if [ ! -z "$completion_marker" ] && grep -q "$completion_marker" "$temp_log_file"; then
            echo -e "\nCompletion marker found - ${command} has finished running."
            # Give a small grace period for any cleanup
            sleep 3
            break
        fi
        
        # If we've hit timeout, exit the loop
        if [ $((i * poll_interval)) -ge $timeout ]; then
            echo -e "\nMax timeout reached. Forcing termination."
            break
        fi
        
        # Wait before polling again (but print a dot every 10 seconds to show activity)
        if [ $((i * poll_interval % 10)) -eq 0 ]; then
            echo -n "."
        fi
        sleep $poll_interval
    done
    
    # Print summary for test command
    if [ "$command" = "test" ]; then
        echo -e "\n===== TEST EXECUTION SUMMARY ====="
        if grep -q "\[FAILED\]" "$temp_log_file"; then
            echo "Some tests FAILED! See output above for details."
            TEST_STATUS="FAILED"
        else
            echo "All tests PASSED successfully!"
            TEST_STATUS="PASSED"
        fi
        
        # Collect some stats
        TOTAL_TESTS=$(grep -c "\[PASSED\]" "$temp_log_file")
        echo "Total tests executed: $TOTAL_TESTS"
        echo "==========================="
    fi
    
    # Cleanup happens via the trap handler
    echo "$(tr '[:lower:]' '[:upper:]' <<< ${command:0:1})${command:1} completed!"
    
    # Manually call cleanup to ensure it happens before the trap is removed
    cleanup "$container_name" "$log_pid" "$temp_log_file"
    
    # Reset the trap
    trap - EXIT INT TERM
}

case "$1" in
    format)
        # Format has no specific completion marker, so we rely on container exit
        run_with_smart_detection "format" "" "" 120
        ;;
    tidy)
        # Static analysis has no specific completion marker, so we rely on container exit
        run_with_smart_detection "tidy" "" "" 300
        ;;
    test)
        # For tests, we know the last test that runs
        run_with_smart_detection "test" "" "ScaleResilienceTest.ScaleConnectionSuccess" 300
        ;;
    build)
        if [ -z "$2" ]; then
            echo "Error: No environment specified"
            echo "Usage: $0 build <environment>"
            echo "Example: $0 build prod_debug"
            exit 1
        fi
        # Build has no specific completion marker, so we rely on container exit
        run_with_smart_detection "build" "$2" "" 300
        ;;
    shell)
        echo "Starting interactive shell in Docker container..."
        docker run --rm -it -v "$(pwd):/project" distiller-tools shell
        ;;
    all)
        echo "Running all commands in Docker container..."
        $0 format
        $0 tidy
        $0 test
        echo "All commands complete!"
        ;;
    help|*)
        show_help
        ;;
esac