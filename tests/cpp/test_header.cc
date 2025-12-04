#include "hpq/gpu/gpu_compress.h"
#include <iostream>

int main() {
  size_t s = hpq::CompressGPU(nullptr, 0, nullptr, 0);
  std::cout << s << std::endl;
  return 0;
}
