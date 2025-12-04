#pragma once

#include "hpq/schema.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace hpq {

class Encoder {
public:
  virtual ~Encoder() = default;

  // Encode values into the internal buffer
  // Returns number of values encoded
  virtual void Put(const void *values, int num_values) = 0;

  // Flush encoded data to the output buffer
  // Returns pointer to data and size
  virtual std::pair<const uint8_t *, size_t> Flush() = 0;

  // Clear internal state
  virtual void Clear() = 0;
};

std::unique_ptr<Encoder> MakePlainEncoder(Type type);

} // namespace hpq
