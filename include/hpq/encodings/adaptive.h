#pragma once

#include "hpq/encodings/encoding_base.h"
#include "hpq/schema.h"
#include <memory>
#include <vector>

namespace hpq {

// AdaptiveEncoder buffers data for a row group (or page),
// analyzes it, and chooses the best encoding (Plain, RLE, BitPack).
class AdaptiveEncoder : public Encoder {
public:
  explicit AdaptiveEncoder(Type type);

  void Put(const void *values, int num_values) override;
  std::pair<const uint8_t *, size_t> Flush() override;
  void Clear() override;

private:
  Type type_;
  // We need to store raw bytes or typed values.
  // For simplicity, let's store raw bytes and cast when needed.
  std::vector<uint8_t> raw_buffer_;
  int num_values_ = 0;

  // The chosen encoder for the current chunk
  std::unique_ptr<Encoder> current_encoder_;

  void DecideAndEncode();
};

} // namespace hpq
