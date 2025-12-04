#include "hpq/encodings/dict_encoding.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

void TestDictEncodingInt64() {
  std::cout << "Testing Dictionary Encoding (INT64)..." << std::endl;

  hpq::DictEncoder encoder(hpq::Type::INT64);

  // Data with high repetition (ideal for dictionary)
  // 0, 100, 0, 100, 200, 0, 100
  std::vector<int64_t> values = {0, 100, 0, 100, 200, 0, 100};

  encoder.Put(values.data(), values.size());
  auto result = encoder.Flush();

  // Expected:
  // Dictionary: [0, 100, 200] (3 entries)
  // Indices: [0, 1, 0, 1, 2, 0, 1]
  // BitWidth: ceil(log2(3)) = 2 bits.

  // Verification is tricky without a decoder, but we can check the size and
  // console output. 3 entries * 8 bytes = 24 bytes + 4 bytes header = 28 bytes
  // for dictionary. 7 indices * 2 bits = 14 bits -> 2 bytes (packed). Total
  // approx: 28 + 1 (width) + 2 = 31 bytes.

  std::cout << "Encoded size: " << result.second << " bytes." << std::endl;

  // Basic sanity check: should be smaller than plain encoding (7 * 8 = 56
  // bytes)
  if (result.second < 56) {
    std::cout << "PASS: Size (" << result.second << ") < Plain (56)"
              << std::endl;
  } else {
    std::cerr << "FAIL: Size (" << result.second << ") >= Plain (56)"
              << std::endl;
    exit(1);
  }
}

int main() {
  TestDictEncodingInt64();
  std::cout << "test_dict passed!" << std::endl;
  return 0;
}
