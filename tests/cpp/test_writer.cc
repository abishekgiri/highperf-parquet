#include "hpq/schema.h"
#include "hpq/writer.h"
#include <cassert>
#include <iostream>
#include <vector>

int main() {
  std::cout << "Running test_writer..." << std::endl;

  // 1. Define Schema
  hpq::Schema schema;
  schema.AddColumn("id", hpq::Type::INT64);
  schema.AddColumn("score", hpq::Type::FLOAT);

  // 2. Initialize Writer
  hpq::ParquetWriter writer("test_output.parquet");
  writer.Init(schema);

  // 3. Prepare Data
  std::vector<int64_t> ids = {1, 2, 3, 4, 5};
  std::vector<float> scores = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f};

  // 4. Write Data
  writer.WriteColumn(0, ids.data(), ids.size());
  writer.WriteColumn(1, scores.data(), scores.size());

  // 5. Close
  writer.Close();

  std::cout << "test_writer passed!" << std::endl;
  return 0;
}
