#include "hpq/bloom_filter.h"
#include <cmath>
#include <cstring>
#include <iostream>

namespace hpq {

// MurmurHash3 64-bit implementation (simplified for standalone use)
static uint64_t MurmurHash3_x64_64(const void *key, int len, uint32_t seed) {
  const uint64_t m = 0xc6a4a7935bd1e995ULL;
  const int r = 47;

  uint64_t h = seed ^ (len * m);

  const uint64_t *data = (const uint64_t *)key;
  const uint64_t *end = data + (len / 8);

  while (data != end) {
    uint64_t k = *data++;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;
  }

  const unsigned char *data2 = (const unsigned char *)data;

  switch (len & 7) {
  case 7:
    h ^= (uint64_t)data2[6] << 48;
    [[fallthrough]];
  case 6:
    h ^= (uint64_t)data2[5] << 40;
    [[fallthrough]];
  case 5:
    h ^= (uint64_t)data2[4] << 32;
    [[fallthrough]];
  case 4:
    h ^= (uint64_t)data2[3] << 24;
    [[fallthrough]];
  case 3:
    h ^= (uint64_t)data2[2] << 16;
    [[fallthrough]];
  case 2:
    h ^= (uint64_t)data2[1] << 8;
    [[fallthrough]];
  case 1:
    h ^= (uint64_t)data2[0];
    h *= m;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}

BloomFilter::BloomFilter(int expected_items, double fpp) {
  // Optimal number of bits: m = -(n * ln(p)) / (ln(2)^2)
  num_bits_ = static_cast<int>(-(expected_items * std::log(fpp)) /
                               (std::pow(std::log(2), 2)));

  // Optimal number of hash functions: k = (m / n) * ln(2)
  num_hashes_ = static_cast<int>((num_bits_ / expected_items) * std::log(2));

  // Ensure minimums
  if (num_bits_ < 64)
    num_bits_ = 64;
  if (num_hashes_ < 1)
    num_hashes_ = 1;

  // Round up to byte boundary
  int num_bytes = (num_bits_ + 7) / 8;
  bitset_.resize(num_bytes, 0);
  num_bits_ = num_bytes * 8; // Adjust to actual size
}

uint64_t BloomFilter::Hash(int64_t value) const {
  return MurmurHash3_x64_64(&value, sizeof(int64_t), 0x9747b28c);
}

uint64_t BloomFilter::Hash(const std::string &value) const {
  return MurmurHash3_x64_64(value.data(), value.size(), 0x9747b28c);
}

void BloomFilter::SetBit(int index) {
  bitset_[index / 8] |= (1 << (index % 8));
}

bool BloomFilter::GetBit(int index) const {
  return (bitset_[index / 8] & (1 << (index % 8))) != 0;
}

void BloomFilter::Insert(int64_t value) {
  uint64_t h = Hash(value);
  uint32_t h1 = h & 0xFFFFFFFF;
  uint32_t h2 = h >> 32;

  for (int i = 0; i < num_hashes_; ++i) {
    // Double hashing: h(i) = (h1 + i * h2) % num_bits
    uint32_t combined = h1 + i * h2;
    SetBit(combined % num_bits_);
  }
}

void BloomFilter::Insert(const std::string &value) {
  uint64_t h = Hash(value);
  uint32_t h1 = h & 0xFFFFFFFF;
  uint32_t h2 = h >> 32;

  for (int i = 0; i < num_hashes_; ++i) {
    uint32_t combined = h1 + i * h2;
    SetBit(combined % num_bits_);
  }
}

bool BloomFilter::Find(int64_t value) const {
  uint64_t h = Hash(value);
  uint32_t h1 = h & 0xFFFFFFFF;
  uint32_t h2 = h >> 32;

  for (int i = 0; i < num_hashes_; ++i) {
    uint32_t combined = h1 + i * h2;
    if (!GetBit(combined % num_bits_)) {
      return false;
    }
  }
  return true;
}

bool BloomFilter::Find(const std::string &value) const {
  uint64_t h = Hash(value);
  uint32_t h1 = h & 0xFFFFFFFF;
  uint32_t h2 = h >> 32;

  for (int i = 0; i < num_hashes_; ++i) {
    uint32_t combined = h1 + i * h2;
    if (!GetBit(combined % num_bits_)) {
      return false;
    }
  }
  return true;
}

std::vector<uint8_t> BloomFilter::Serialize() const { return bitset_; }

} // namespace hpq
