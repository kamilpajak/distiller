#!/bin/bash

echo "Running tests for Distiller project..."
echo

echo "Building and running tests..."
pio test -e test

echo
echo "Test execution completed."
