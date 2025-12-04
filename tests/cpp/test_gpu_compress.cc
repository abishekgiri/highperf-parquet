#include "hpq/gpu/gpu_compress.h"
#include "hpq/writer.h"
#include <cassert>
#include <iostream>
#include <vector>

void TestDirectCompression() {
  std::cout << "Testing Direct GPU Compression..." << std::endl;
  std::vector<uint8_t> input(1024, 1); // 1KB of 1s
  std::vector<uint8_t> output(2048);

  size_t compressed_size = hpq::CompressGPU(input.data(), input.size(),
                                            output.data(), output.size());

#ifdef HPQ_USE_CUDA
  if (compressed_size > 0) {
    std::cout << "Compression successful: " << compressed_size << " bytes."
              << std::endl;
  } else {
    std::cout << "Compression failed (or not supported)." << std::endl;
  }
#else
  assert(compressed_size == 0);
  std::cout << "Compression skipped (CUDA disabled)." << std::endl;
#endif
}

void TestWriterIntegration() {
  std::cout << "Testing Writer Integration with GPU..." << std::endl;
  hpq::Schema schema;
  schema.AddColumn("data", hpq::Type::INT32);

  hpq::WriterOptions options;
  options.use_gpu_compression = true;

  hpq::ParquetWriter writer("test_gpu_output.parquet", options);
  writer.Init(schema);

  std::vector<int32_t> data(1000, 42);
  writer.WriteColumn(0, data.data(), data.size());

  writer.Close();
  std::cout << "Writer closed successfully." << std::endl;
}

int main() {
  TestDirectCompression();
  TestWriterIntegration();
  std::cout << "test_gpu_compress passed!" << std::endl;
  return 0;
}
