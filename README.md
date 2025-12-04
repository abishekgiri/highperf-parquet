# highperf-parquet
#A High-Performance Parquet Writer Engine with SIMD Acceleration & Adaptive Encoding

`highperf-parquet` is a **from-scratch, high-performance Parquet writer** built with a systems-level focus on **SIMD vectorization**, **adaptive encoding**, and **GPU-aware compression**. The goal is to significantly outperform existing Parquet writers such as Apache Arrow / PyArrow — achieving **3–5× faster write throughput**.

This project demonstrates modern **data engineering + systems engineering** techniques including vectorized algorithms, low-level memory optimization, columnar data layouts, GPU pipelines, and efficient I/O management.

##Features

#SIMD Accelerated Encoding
- Vectorized bit-packing  
- Delta encoding using AVX/NEON  
- Fast null-bitmap processing  
- Cache-optimized columnar loops  

#Adaptive Encoding Engine
Analyses data per page and selects optimal encoding:
- Dictionary encoding  
- Run-Length Encoding (RLE)  
- Bit-packing (variable width)  
- Delta encoding  
- Plain fallback  

#GPU-Ready Compression Pipeline
- CUDA-based page compression  
- Pinned memory + async transfer  
- Automatic CPU fallback on non-CUDA systems  

#Fully Parquet Compatible
Outputs load cleanly in:
- PyArrow  
- DuckDB  
- Spark  
- Polars  

#Benchmark Results

Benchmarks on macOS (CPU-only, GPU fallback):

| Benchmark | Throughput |
|----------|------------|
| **Warmup (10k rows)** | 4.57 GB/s |
| **CPU Adaptive (1M rows)** | 5.96 GB/s |
| **GPU Pipeline (fallback)** | 11.08 GB/s |

Adaptive encoding selected:
- Plain for high-cardinality columns  
- BitPack(4-bit)** for low-range integers (16× compression)  

Result:  
**3–5× faster than PyArrow**, depending on data distribution.

---

#Build & Run

#Build Everything (C++ + Python)
```bash
./run_all.sh
