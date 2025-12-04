#pragma once

#include "hpq/schema.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace hpq {

struct WriterOptions {
  size_t row_group_size = 64 * 1024;
  bool use_dictionary = true;
  bool use_gpu_compression = false;
  std::string compression = "SNAPPY"; // SNAPPY, GZIP, ZSTD, NONE
};

class ParquetWriter {
public:
  explicit ParquetWriter(const std::string &filename,
                         const WriterOptions &options = WriterOptions());
  ~ParquetWriter();

  // Disable copy
  ParquetWriter(const ParquetWriter &) = delete;
  ParquetWriter &operator=(const ParquetWriter &) = delete;

  // Initialize the writer with a schema
  void Init(const Schema &schema);

  // Write a column chunk
  void WriteColumn(int col_idx, const void *values, int num_values);

  void Close();

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace hpq
