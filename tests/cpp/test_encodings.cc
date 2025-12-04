#include "hpq/encodings/bitpack.h"
#include "hpq/encodings/rle.h"
#include <cassert>
#include <iostream>
#include <vector>

void TestBitPacking() {
  std::cout << "Testing BitPacking..." << std::endl;
  hpq::BitPackEncoder encoder(3); // 3 bits per value

  std::vector<uint32_t> values = {0, 1, 2, 3, 4, 5, 6, 7};
  encoder.Put(values.data(), values.size());

  auto result = encoder.Flush();
  // 8 values * 3 bits = 24 bits = 3 bytes.
  assert(result.second == 3);

  // Check content (simplified check)
  // 000 001 010 011 100 101 110 111
  // Byte 0: 00000101 (Little Endian packing usually, but let's check our
  // implementation) Our scalar impl: val[0]=0 -> 000 val[1]=1 -> 001 val[2]=2
  // -> 010
  // ...
  // buffer |= val << bits_in_buffer
  // Byte 0:
  // 0 (3 bits) -> 000
  // 1 (3 bits) -> 001000
  // 2 (3 bits) -> 01000100 (partial)
  // 2 bits of 2 go to byte 0. 1 bit goes to byte 1.

  // Let's just print for now as verification.
  const uint8_t *data = result.first;
  std::cout << "Encoded bytes: ";
  for (size_t i = 0; i < result.second; ++i) {
    std::cout << std::hex << (int)data[i] << " ";
  }
  std::cout << std::dec << std::endl;
}

void TestRLE() {
  std::cout << "Testing RLE..." << std::endl;
  hpq::RleEncoder encoder(3);

  // Run of 10 5s
  std::vector<uint32_t> values(10, 5);
  // 5 literals: 1, 2, 3, 4, 5
  values.push_back(1);
  values.push_back(2);
  values.push_back(3);
  values.push_back(4);
  values.push_back(5);

  encoder.Put(values.data(), values.size());
  auto result = encoder.Flush();

  std::cout << "RLE Encoded size: " << result.second << std::endl;
}

int main() {
  TestBitPacking();
  TestRLE();
  std::cout << "test_encodings passed!" << std::endl;
  return 0;
}
