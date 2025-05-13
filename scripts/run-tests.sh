#!/bin/bash
set -e
echo "Running tests..."
pio test -e test
# Ensure all processes are terminated
killall -9 pio 2>/dev/null || true
# Exit explicitly with the status of the test command
exit ${PIPESTATUS[0]}
