#include "hpq/schema.h"
#include "hpq/writer.h"
#include <iostream>
#include <numeric>
#include <vector>

void TestAdaptiveSelection() {
  std::cout << "Testing Adaptive Encoding Selection..." << std::endl;

  hpq::Schema schema;
  schema.AddColumn("rle_col", hpq::Type::INT32);
  schema.AddColumn("bitpack_col", hpq::Type::INT32);
  schema.AddColumn("plain_col", hpq::Type::INT32);

  hpq::WriterOptions options;
  hpq::ParquetWriter writer("test_adaptive.parquet", options);
  writer.Init(schema);

  // 1. RLE Candidate: Many repeated values
  std::vector<int32_t> rle_data(1000, 42); // 1000 42s
  writer.WriteColumn(0, rle_data.data(), rle_data.size());

  // 2. BitPack Candidate: Small values, no runs
  std::vector<int32_t> bitpack_data(1000);
  for (int i = 0; i < 1000; ++i)
    bitpack_data[i] = i % 8; // 0-7, fits in 3 bits
  writer.WriteColumn(1, bitpack_data.data(), bitpack_data.size());

  // 3. Plain Candidate: Large random-ish values
  std::vector<int32_t> plain_data(1000);
  std::iota(plain_data.begin(), plain_data.end(), 1000000);
  writer.WriteColumn(2, plain_data.data(), plain_data.size());

  writer.Close();
  // Check stdout for "Adaptive: Selected ..." messages
}

int main() {
  TestAdaptiveSelection();
  return 0;
}
