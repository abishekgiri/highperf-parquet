#!/bin/bash
set -e

echo "========================================"
echo "   High-Performance Parquet Writer"
echo "========================================"

# 1. Build
echo "[1/3] Building project..."
mkdir -p build
cd build
cmake -G Ninja ..
ninja

# 2. Run Tests
echo ""
echo "[2/3] Running Verification Tests..."
ctest --output-on-failure

# 3. Run Benchmarks
echo ""
echo "[3/3] Running Performance Benchmarks..."
./benchmark_writer

echo ""
echo "========================================"
echo "   All functions verified and run!"
echo "========================================"
