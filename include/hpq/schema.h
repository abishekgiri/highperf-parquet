#pragma once

#include <memory>
#include <string>
#include <vector>

namespace hpq {

enum class Type {
  BOOLEAN,
  INT32,
  INT64,
  FLOAT,
  DOUBLE,
  BYTE_ARRAY,
  FIXED_LEN_BYTE_ARRAY
};

struct ColumnSchema {
  std::string name;
  Type type;
  bool nullable;
  int type_length = 0; // For FIXED_LEN_BYTE_ARRAY
};

class Schema {
public:
  Schema() = default;
  void AddColumn(const std::string &name, Type type, bool nullable = true);

  const std::vector<ColumnSchema> &columns() const { return columns_; }
  size_t num_columns() const { return columns_.size(); }

private:
  std::vector<ColumnSchema> columns_;
};

} // namespace hpq
