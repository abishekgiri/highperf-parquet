#pragma once

#include "hpq/encodings/bitpack.h"
#include "hpq/encodings/encoding_base.h"

namespace hpq {

class RleEncoder : public Encoder {
public:
  explicit RleEncoder(int bit_width);

  void Put(const void *values, int num_values) override;
  std::pair<const uint8_t *, size_t> Flush() override;
  void Clear() override;

private:
  int bit_width_;
  std::vector<uint8_t> buffer_;
  std::unique_ptr<BitPackEncoder> bit_packer_;

  // RLE state
  uint32_t current_value_ = 0;
  int run_length_ = 0;
  std::vector<uint32_t> literal_buffer_;

  void FlushRun();
  void FlushLiterals();
};

} // namespace hpq
