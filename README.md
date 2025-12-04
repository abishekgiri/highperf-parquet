# High-Performance Parquet/ORC Writer

A high-performance Parquet writer built from scratch with C++ and CUDA, featuring SIMD-optimized encoders and GPU-offloaded compression.

## Goals
- 3-5x throughput vs Apache Arrow
- SIMD-accelerated encoding (AVX2/AVX-512)
- GPU-offloaded compression (CUDA)
- Adaptive encoding strategy

## Run Everything
To build, test, and benchmark the entire project in one go:
```bash
./run_all.sh
```

## Manual Build
```bash
mkdir build && cd build
cmake -G Ninja ..
ninja

## Running
### Benchmarks
To see the performance (throughput in MB/s):
```bash
./benchmark_writer
```

### Tests
To verify correctness of all encoders (Adaptive, Dict, Delta, Bloom, etc.):
```bash
ctest --verbose
```
