#include "hpq/encodings/adaptive.h"
#include "hpq/encodings/bitpack.h"
#include "hpq/encodings/rle.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

namespace hpq {

AdaptiveEncoder::AdaptiveEncoder(Type type) : type_(type) {}

void AdaptiveEncoder::Put(const void *values, int num_values) {
  int type_size = 0;
  switch (type_) {
  case Type::INT32:
    type_size = 4;
    break;
  case Type::INT64:
    type_size = 8;
    break;
  case Type::FLOAT:
    type_size = 4;
    break;
  case Type::DOUBLE:
    type_size = 8;
    break;
  case Type::BOOLEAN:
    type_size = 1;
    break; // Simplified
  default:
    type_size = 1;
    break;
  }

  size_t current_size = raw_buffer_.size();
  size_t bytes_to_add = num_values * type_size;
  raw_buffer_.resize(current_size + bytes_to_add);
  std::memcpy(raw_buffer_.data() + current_size, values, bytes_to_add);
  num_values_ += num_values;
}

void AdaptiveEncoder::DecideAndEncode() {
  // Simple heuristic:
  // 1. Calculate min/max to see if BitPacking is viable (for INT).
  // 2. Check for runs to see if RLE is viable.
  // 3. Default to Plain.

  if (type_ == Type::INT32) {
    const int32_t *values =
        reinterpret_cast<const int32_t *>(raw_buffer_.data());
    int32_t min_val = values[0];
    int32_t max_val = values[0];

    // Check runs
    int max_run_length = 0;
    int current_run = 1;
    for (int i = 1; i < num_values_; ++i) {
      if (values[i] < min_val)
        min_val = values[i];
      if (values[i] > max_val)
        max_val = values[i];

      if (values[i] == values[i - 1]) {
        current_run++;
      } else {
        if (current_run > max_run_length)
          max_run_length = current_run;
        current_run = 1;
      }
    }
    if (current_run > max_run_length)
      max_run_length = current_run;

    // Heuristics
    bool use_rle = (max_run_length > 10); // Arbitrary threshold

    // Check bit width for packing
    // Only valid if non-negative for simple bitpacking (or use zigzag)
    // Our BitPackEncoder is simple, assumes unsigned or small positive.
    bool use_bitpack = false;
    int bit_width = 32;
    if (min_val >= 0) {
      if (max_val == 0)
        bit_width = 0;
      else
        bit_width = static_cast<int>(std::ceil(std::log2(max_val + 1)));

      if (bit_width < 28)
        use_bitpack = true; // Saving at least 4 bits/value
    }

    if (use_rle) {
      // RLE wins for high repetition
      // Note: Our RLE implementation currently uses BitPacking internally too.
      // Ideally we pass the bit_width to RLE.
      int width = (min_val >= 0) ? bit_width : 32;
      current_encoder_ = std::make_unique<RleEncoder>(width);
      std::cout << "Adaptive: Selected RLE (width=" << width << ")"
                << std::endl;
    } else if (use_bitpack) {
      current_encoder_ = std::make_unique<BitPackEncoder>(bit_width);
      std::cout << "Adaptive: Selected BitPack (width=" << bit_width << ")"
                << std::endl;
    } else {
      current_encoder_ = MakePlainEncoder(type_);
      std::cout << "Adaptive: Selected Plain" << std::endl;
    }

  } else {
    // Default for other types
    current_encoder_ = MakePlainEncoder(type_);
    std::cout << "Adaptive: Selected Plain (Default)" << std::endl;
  }

  // Encode
  // Note: Our encoders expect typed pointers.
  // We have raw bytes.
  // For INT32, raw_buffer_.data() is correct.
  current_encoder_->Put(raw_buffer_.data(), num_values_);
}

std::pair<const uint8_t *, size_t> AdaptiveEncoder::Flush() {
  if (num_values_ == 0)
    return {nullptr, 0};

  DecideAndEncode();
  auto result = current_encoder_->Flush();

  return result;
}

void AdaptiveEncoder::Clear() {
  raw_buffer_.clear();
  num_values_ = 0;
  if (current_encoder_) {
    current_encoder_->Clear();
    current_encoder_.reset();
  }
}

} // namespace hpq
