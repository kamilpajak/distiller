#!/bin/bash
set -e
echo "Running clang-format..."
find src include lib test \( -name "*.h" -o -name "*.cpp" \) | xargs clang-format -i -style=file
echo "Formatting complete!"