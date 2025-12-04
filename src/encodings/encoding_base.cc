#include "hpq/encodings/encoding_base.h"
#include <cstring>
#include <stdexcept>

namespace hpq {

template <typename T> class PlainEncoder : public Encoder {
public:
  void Put(const void *values, int num_values) override {
    const T *input = static_cast<const T *>(values);
    size_t current_size = buffer_.size();
    size_t bytes_to_add = num_values * sizeof(T);
    buffer_.resize(current_size + bytes_to_add);
    std::memcpy(buffer_.data() + current_size, input, bytes_to_add);
  }

  std::pair<const uint8_t *, size_t> Flush() override {
    return {buffer_.data(), buffer_.size()};
  }

  void Clear() override { buffer_.clear(); }

private:
  std::vector<uint8_t> buffer_;
};

// Specialization for BYTE_ARRAY (strings) could go here, but keeping it simple
// for now.

std::unique_ptr<Encoder> MakePlainEncoder(Type type) {
  switch (type) {
  case Type::INT32:
    return std::make_unique<PlainEncoder<int32_t>>();
  case Type::INT64:
    return std::make_unique<PlainEncoder<int64_t>>();
  case Type::FLOAT:
    return std::make_unique<PlainEncoder<float>>();
  case Type::DOUBLE:
    return std::make_unique<PlainEncoder<double>>();
  case Type::BOOLEAN:
    return std::make_unique<PlainEncoder<
        uint8_t>>(); // Boolean as 1 byte for now (Parquet uses bitpacking
                     // usually but PLAIN can be byte-wise for simplicity in
                     // some contexts, though spec says PLAIN for boolean is
                     // bitpacked. Let's stick to simple byte storage for this
                     // "Plain" encoder for now and refine later or use
                     // BitPacking encoder for bools properly).
  // Actually, Parquet PLAIN encoding for BOOLEAN *is* bit packed.
  // For this "Minimal" phase, let's treat it as bytes and fix it when we do the
  // actual Parquet format compliance check.
  default:
    throw std::runtime_error("Unsupported type for PlainEncoder");
  }
}

} // namespace hpq
