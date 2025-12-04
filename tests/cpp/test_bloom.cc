#include "hpq/bloom_filter.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

void TestBloomFilter() {
  std::cout << "Testing Bloom Filter..." << std::endl;

  // Create a filter for 100 items with 5% false positive probability
  hpq::BloomFilter filter(100, 0.05);

  // Insert some values
  for (int i = 0; i < 100; ++i) {
    filter.Insert(i);
  }
  filter.Insert("Hello");
  filter.Insert("World");

  // Verify existence (True Positives)
  for (int i = 0; i < 100; ++i) {
    if (!filter.Find(i)) {
      std::cerr << "FAIL: False Negative for " << i << std::endl;
      exit(1);
    }
  }
  if (!filter.Find("Hello")) {
    std::cerr << "FAIL: False Negative for Hello" << std::endl;
    exit(1);
  }

  // Verify non-existence (True Negatives / False Positives)
  int false_positives = 0;
  int trials = 1000;
  for (int i = 1000; i < 1000 + trials; ++i) {
    if (filter.Find(i)) {
      false_positives++;
    }
  }

  double fp_rate = static_cast<double>(false_positives) / trials;
  std::cout << "False Positive Rate: " << fp_rate * 100 << "% (Expected < 5%)"
            << std::endl;

  if (fp_rate > 0.10) { // Allow some margin
    std::cerr << "FAIL: FP Rate too high" << std::endl;
    exit(1);
  }

  std::cout << "PASS: Bloom Filter verified." << std::endl;
}

int main() {
  TestBloomFilter();
  return 0;
}
