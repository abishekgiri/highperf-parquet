#include "hpq/encodings/dict_encoding.h"
#include <cmath>
#include <cstring>
#include <iostream>

namespace hpq {

DictEncoder::DictEncoder(Type type) : type_(type) {}

void DictEncoder::Put(const void *values, int num_values) {
  if (type_ == Type::INT64) {
    const int64_t *input = static_cast<const int64_t *>(values);
    for (int i = 0; i < num_values; ++i) {
      int64_t val = input[i];
      if (dict_int64_.find(val) == dict_int64_.end()) {
        int32_t new_index = static_cast<int32_t>(dict_values_int64_.size());
        dict_int64_[val] = new_index;
        dict_values_int64_.push_back(val);
        indices_.push_back(new_index);
      } else {
        indices_.push_back(dict_int64_[val]);
      }
    }
  }
  // TODO: Handle other types
}

std::pair<const uint8_t *, size_t> DictEncoder::Flush() {
  buffer_.clear();

  // 1. Write Dictionary Page (The actual values)
  // In a real Parquet file, this is a separate page.
  // For this writer, we'll prefix the data with the dictionary for simplicity
  // of the demo. Format: [NumEntries: 4 bytes] [Value1] [Value2] ... [BitWidth:
  // 1 byte] [RLE/BitPacked Indices]

  int32_t num_entries = static_cast<int32_t>(dict_values_int64_.size());
  buffer_.resize(sizeof(int32_t) + num_entries * sizeof(int64_t));

  std::memcpy(buffer_.data(), &num_entries, sizeof(int32_t));
  std::memcpy(buffer_.data() + sizeof(int32_t), dict_values_int64_.data(),
              num_entries * sizeof(int64_t));

  // 2. Encode Indices
  // Calculate required bit width
  int bit_width = 1;
  if (num_entries > 0) {
    // bit_width = ceil(log2(num_entries + 1)) ? No, just log2(num_entries) if
    // 0-indexed. E.g., 2 entries (0, 1) -> 1 bit. 3 entries (0, 1, 2) -> 2
    // bits. width = ceil(log2(num_entries))
    if (num_entries > 1) {
      bit_width = static_cast<int>(std::ceil(std::log2(num_entries)));
    }
  }

  // Use our BitPackEncoder or RleEncoder for indices
  // Let's use RleEncoder as it handles bit-packing internally usually,
  // but our RleEncoder is simple. Let's use BitPackEncoder for indices as it's
  // SIMD optimized now!

  BitPackEncoder index_encoder(bit_width);
  // BitPackEncoder expects uint32_t
  // indices_ are int32_t, safe to cast
  index_encoder.Put(indices_.data(), indices_.size());
  auto encoded_indices = index_encoder.Flush();

  // Append BitWidth
  uint8_t bw_byte = static_cast<uint8_t>(bit_width);
  buffer_.push_back(bw_byte);

  // Append Encoded Indices
  size_t dict_offset = buffer_.size();
  buffer_.resize(dict_offset + encoded_indices.second);
  std::memcpy(buffer_.data() + dict_offset, encoded_indices.first,
              encoded_indices.second);

  std::cout << "DictEncoder: " << indices_.size() << " values -> "
            << num_entries << " unique entries." << std::endl;

  return {buffer_.data(), buffer_.size()};
}

void DictEncoder::Clear() {
  dict_int64_.clear();
  dict_values_int64_.clear();
  indices_.clear();
  buffer_.clear();
}

} // namespace hpq
