#include "hpq/encodings/delta.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

namespace hpq {

DeltaEncoder::DeltaEncoder(Type type) : type_(type) {}

void DeltaEncoder::Put(const void *values, int num_values) {
  if (type_ == Type::INT64) {
    const int64_t *input = static_cast<const int64_t *>(values);
    buffered_values_.insert(buffered_values_.end(), input, input + num_values);
  } else if (type_ == Type::INT32) {
    const int32_t *input = static_cast<const int32_t *>(values);
    for (int i = 0; i < num_values; ++i) {
      buffered_values_.push_back(static_cast<int64_t>(input[i]));
    }
  }
}

// Helper to calculate ZigZag encoding for signed integers
// Maps signed range to unsigned range: 0->0, -1->1, 1->2, -2->3...
static uint64_t ZigZagEncode(int64_t n) { return (n << 1) ^ (n >> 63); }

// Helper to write ULEB128 (Unsigned Little Endian Base 128)
static void WriteULEB128(std::vector<uint8_t> &buf, uint64_t val) {
  do {
    uint8_t byte = val & 0x7F;
    val >>= 7;
    if (val != 0) {
      byte |= 0x80;
    }
    buf.push_back(byte);
  } while (val != 0);
}

std::pair<const uint8_t *, size_t> DeltaEncoder::Flush() {
  buffer_.clear();
  if (buffered_values_.empty()) {
    return {buffer_.data(), 0};
  }

  // Delta Encoding Format (Simplified Parquet)
  // <BlockSize> <NumMiniBlocks> <TotalValueCount> <FirstValue>
  // Then blocks of data...

  // 1. Header
  int block_size = 128;    // Standard block size
  int num_mini_blocks = 4; // 4 mini blocks per block (32 values each)
  int total_count = buffered_values_.size();
  int64_t first_value = buffered_values_[0];

  WriteULEB128(buffer_, block_size);
  WriteULEB128(buffer_, num_mini_blocks);
  WriteULEB128(buffer_, total_count);
  WriteULEB128(buffer_,
               ZigZagEncode(first_value)); // First value is ZigZag encoded

  // 2. Process Blocks
  int64_t current_value = first_value;
  for (size_t i = 1; i < buffered_values_.size(); i += block_size) {
    // For each block
    // Calculate min delta
    int64_t min_delta = INT64_MAX;
    std::vector<int64_t> deltas;

    size_t end = std::min(i + block_size, buffered_values_.size());
    for (size_t j = i; j < end; ++j) {
      int64_t delta = buffered_values_[j] - current_value;
      deltas.push_back(delta);
      if (delta < min_delta)
        min_delta = delta;
      current_value = buffered_values_[j];
    }

    // Write Min Delta (ZigZag ULEB128)
    WriteULEB128(buffer_, ZigZagEncode(min_delta));

    // Compute bit widths for mini blocks
    // For simplicity in this demo, we'll use one bit width for the whole block
    // or just assume 32 values per mini block.

    // Subtract min_delta from all deltas to make them positive
    uint64_t max_adjusted_delta = 0;
    for (auto &d : deltas) {
      d -= min_delta; // Now d >= 0
      if (static_cast<uint64_t>(d) > max_adjusted_delta)
        max_adjusted_delta = d;
    }

    // Calculate bit width
    int bit_width = 0;
    if (max_adjusted_delta > 0) {
      bit_width = 64 - __builtin_clzll(max_adjusted_delta);
    }

    // Write bit widths for mini blocks (4 bytes, one per mini block)
    // Since we use uniform bit width for the whole block in this simplified
    // version:
    for (int m = 0; m < num_mini_blocks; ++m)
      buffer_.push_back(bit_width);

    // BitPack the adjusted deltas
    // We can reuse BitPackEncoder!
    BitPackEncoder packer(bit_width);
    // BitPackEncoder expects uint32_t, but our deltas might be larger.
    // Our current BitPackEncoder is limited to 32-bit.
    // For this demo, let's assume deltas fit in 32-bit (reasonable for most
    // cases).

    std::vector<uint32_t> deltas_u32;
    for (auto d : deltas)
      deltas_u32.push_back(static_cast<uint32_t>(d));

    packer.Put(deltas_u32.data(), deltas_u32.size());
    auto packed = packer.Flush();

    buffer_.insert(buffer_.end(), packed.first, packed.first + packed.second);
  }

  return {buffer_.data(), buffer_.size()};
}

void DeltaEncoder::Clear() {
  buffered_values_.clear();
  buffer_.clear();
}

} // namespace hpq
