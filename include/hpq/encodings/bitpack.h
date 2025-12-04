#pragma once

#include "hpq/encodings/encoding_base.h"

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#endif

namespace hpq {

class BitPackEncoder : public Encoder {
public:
  explicit BitPackEncoder(int bit_width);

  void Put(const void *values, int num_values) override;
  std::pair<const uint8_t *, size_t> Flush() override;
  void Clear() override;

private:
  int bit_width_;
  std::vector<uint8_t> buffer_;
  std::vector<uint32_t> buffered_values_; // Temp buffer for packing

  void FlushBufferedValues();
  void Pack8Values(const uint32_t *in, uint8_t *out);
};

} // namespace hpq
