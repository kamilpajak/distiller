#!/bin/bash
set -e
echo "Running clang-tidy..."
find src include lib/*/src lib/*/include test -name "*.cpp" | xargs clang-tidy -p . --config-file=.clang-tidy
echo "Linting complete!"