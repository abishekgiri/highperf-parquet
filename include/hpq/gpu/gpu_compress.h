#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace hpq {

// Compress data using GPU
// Returns compressed size, or 0 if failed/not supported
size_t CompressGPU(const uint8_t *input, size_t input_size, uint8_t *output,
                   size_t output_capacity);

} // namespace hpq
