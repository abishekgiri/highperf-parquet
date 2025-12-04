#pragma once

#include "hpq/encodings/bitpack.h"
#include "hpq/encodings/encoding_base.h"
#include <vector>

namespace hpq {

// Delta Binary Packed Encoding
// Supported Types: INT32, INT64
class DeltaEncoder : public Encoder {
public:
  explicit DeltaEncoder(Type type);

  void Put(const void *values, int num_values) override;
  std::pair<const uint8_t *, size_t> Flush() override;
  void Clear() override;

private:
  Type type_;
  std::vector<int64_t>
      buffered_values_; // Store as int64 to cover both int32/int64
  std::vector<uint8_t> buffer_;

  // Helper to encode a block of integers
  void EncodeBlock(const int64_t *values, int count);
};

} // namespace hpq
