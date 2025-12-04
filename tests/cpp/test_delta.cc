#include "hpq/encodings/delta.h"
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

void TestDeltaEncoding() {
  std::cout << "Testing Delta Encoding..." << std::endl;

  hpq::DeltaEncoder encoder(hpq::Type::INT64);

  // Sequential data: 1000, 1001, 1002...
  // Deltas: 1, 1, 1... (very small, 1 bit)
  int num_values = 1000;
  std::vector<int64_t> values(num_values);
  std::iota(values.begin(), values.end(), 1000);

  encoder.Put(values.data(), values.size());
  auto result = encoder.Flush();

  // Expected size:
  // Header ~ 5-10 bytes
  // Block:
  //   Min Delta (1 byte)
  //   Bit Widths (4 bytes)
  //   Data: 1000 values * 1 bit = 1000 bits = 125 bytes.
  // Total should be around 140-150 bytes.
  // Plain encoding would be 1000 * 8 = 8000 bytes.

  std::cout << "Encoded size: " << result.second << " bytes." << std::endl;

  if (result.second < 200) {
    std::cout << "PASS: Size (" << result.second << ") << Plain (8000)"
              << std::endl;
  } else {
    std::cerr << "FAIL: Size (" << result.second << ") is too large"
              << std::endl;
    exit(1);
  }
}

int main() {
  TestDeltaEncoding();
  std::cout << "test_delta passed!" << std::endl;
  return 0;
}
