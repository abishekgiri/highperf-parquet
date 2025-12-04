#include "hpq/gpu/gpu_compress.h"

namespace hpq {

size_t CompressGPU(const uint8_t *input, size_t input_size, uint8_t *output,
                   size_t output_capacity) {
  // Fallback implementation (no-op)
  // In a real scenario, we might fallback to CPU compression (Snappy/LZ4) here.
  return 0;
}

} // namespace hpq
