#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace hpq {

// Bloom Filter implementation
// Uses a simple bitset and multiple hash functions (simulated via double
// hashing)
class BloomFilter {
public:
  // Initialize with expected number of items and desired false positive
  // probability
  BloomFilter(int expected_items, double fpp = 0.05);

  void Insert(int64_t value);
  void Insert(const std::string &value);

  bool Find(int64_t value) const;
  bool Find(const std::string &value) const;

  // Serialize to buffer (Parquet Bloom Filter format is complex, we'll use a
  // simple bitset dump for this demo)
  std::vector<uint8_t> Serialize() const;

private:
  std::vector<uint8_t> bitset_;
  int num_bits_;
  int num_hashes_;

  // Hash functions
  uint64_t Hash(int64_t value) const;
  uint64_t Hash(const std::string &value) const;

  void SetBit(int index);
  bool GetBit(int index) const;
};

} // namespace hpq
