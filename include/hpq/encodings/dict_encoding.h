#pragma once

#include "hpq/encodings/bitpack.h"
#include "hpq/encodings/encoding_base.h"
#include "hpq/encodings/rle.h"
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace hpq {

// Dictionary Encoder for generic types (currently specialized for
// strings/integers) For simplicity in this advanced phase, we'll focus on INT64
// and Strings (simulated as byte arrays).
class DictEncoder : public Encoder {
public:
  explicit DictEncoder(Type type);

  void Put(const void *values, int num_values) override;
  std::pair<const uint8_t *, size_t> Flush() override;
  void Clear() override;

private:
  Type type_;

  // Dictionary: Value -> Index
  // We use a variant to handle different key types for the map
  // For simplicity/performance in this demo, we'll use separate maps or a
  // template approach. Let's use a template-like approach with void* casting
  // for the "Advanced" demo, or just support INT64 for now to demonstrate the
  // mechanics without template bloat. Actually, let's do INT64.

  std::unordered_map<int64_t, int32_t> dict_int64_;
  std::vector<int64_t> dict_values_int64_;

  // Indices of the data
  std::vector<int32_t> indices_;

  // We need an RLE/BitPack encoder for the indices
  // The bit width for indices depends on the dictionary size.
  std::vector<uint8_t> buffer_;
};

} // namespace hpq
