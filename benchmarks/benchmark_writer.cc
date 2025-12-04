#include "hpq/schema.h"
#include "hpq/writer.h"
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

void RunBenchmark(const std::string &name, int num_rows, bool use_gpu) {
  std::cout << "Running Benchmark: " << name << " (" << num_rows << " rows)"
            << std::endl;

  hpq::Schema schema;
  schema.AddColumn("id", hpq::Type::INT64);
  schema.AddColumn("val_random", hpq::Type::INT64);
  schema.AddColumn("val_repeat", hpq::Type::INT64);
  schema.AddColumn("val_small", hpq::Type::INT32);

  hpq::WriterOptions options;
  options.use_gpu_compression = use_gpu;

  // Generate Data
  std::vector<int64_t> col_id(num_rows);
  std::iota(col_id.begin(), col_id.end(), 0);

  std::vector<int64_t> col_random(num_rows);
  std::mt19937_64 rng(42);
  std::uniform_int_distribution<int64_t> dist(0, 1000000);
  for (auto &v : col_random)
    v = dist(rng);

  std::vector<int64_t> col_repeat(num_rows, 12345);

  std::vector<int32_t> col_small(num_rows);
  for (int i = 0; i < num_rows; ++i)
    col_small[i] = i % 16;

  auto start = std::chrono::high_resolution_clock::now();

  hpq::ParquetWriter writer("benchmark_out.parquet", options);
  writer.Init(schema);

  writer.WriteColumn(0, col_id.data(), num_rows);
  writer.WriteColumn(1, col_random.data(), num_rows);
  writer.WriteColumn(2, col_repeat.data(), num_rows);
  writer.WriteColumn(3, col_small.data(), num_rows);

  writer.Close();

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  size_t total_bytes = num_rows * (8 + 8 + 8 + 4);
  double mb = total_bytes / (1024.0 * 1024.0);
  double throughput = mb / elapsed.count();

  std::cout << "Time: " << elapsed.count() << "s" << std::endl;
  std::cout << "Throughput: " << throughput << " MB/s" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}

int main() {
  // Warmup
  RunBenchmark("Warmup", 10000, false);

  // Run with 1M rows
  RunBenchmark("CPU Adaptive", 1000000, false);

  // Run with GPU (Fallback on Mac)
  RunBenchmark("GPU (Fallback)", 1000000, true);

  return 0;
}
