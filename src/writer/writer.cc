#include "hpq/writer.h"
#include "hpq/encodings/adaptive.h"
#include "hpq/encodings/encoding_base.h"
#include "hpq/gpu/gpu_compress.h"
#include <iostream>
#include <vector>

namespace hpq {

class ParquetWriter::Impl {
public:
  Impl(const std::string &filename, const WriterOptions &options)
      : filename_(filename), options_(options) {}

  void Init(const Schema &schema) {
    schema_ = schema;
    for (const auto &col : schema.columns()) {
      // Use AdaptiveEncoder for all columns for now
      encoders_.push_back(std::make_unique<AdaptiveEncoder>(col.type));
    }
  }

  void WriteColumn(int col_idx, const void *values, int num_values) {
    if (col_idx < 0 || col_idx >= static_cast<int>(encoders_.size())) {
      return;
    }
    encoders_[col_idx]->Put(values, num_values);
  }

  void Close() {
    std::cout << "Closing writer for " << filename_ << std::endl;
    for (size_t i = 0; i < encoders_.size(); ++i) {
      auto result = encoders_[i]->Flush();

      // Compression
      std::vector<uint8_t> compressed_buffer;
      const uint8_t *data_to_write = result.first;
      size_t size_to_write = result.second;

      if (options_.use_gpu_compression) {
        // Allocate enough space for worst case
        compressed_buffer.resize(size_to_write + 1024);
        size_t compressed_size = hpq::CompressGPU(result.first, result.second,
                                                  compressed_buffer.data(),
                                                  compressed_buffer.size());

        if (compressed_size > 0) {
          std::cout << "Column " << i << " GPU Compressed: " << result.second
                    << " -> " << compressed_size << " bytes." << std::endl;
          data_to_write = compressed_buffer.data();
          size_to_write = compressed_size;
        } else {
          std::cout << "Column " << i
                    << " GPU Compression failed/unsupported. Using raw."
                    << std::endl;
        }
      } else {
        std::cout << "Column " << i << " encoded " << result.second << " bytes."
                  << std::endl;
      }

      // In real writer, we would write 'data_to_write' of 'size_to_write' to
      // disk here.
      (void)data_to_write; // Silence unused warning
      (void)size_to_write;
    }
  }

private:
  std::string filename_;
  WriterOptions options_;
  Schema schema_;
  std::vector<std::unique_ptr<Encoder>> encoders_;
};

ParquetWriter::ParquetWriter(const std::string &filename,
                             const WriterOptions &options)
    : impl_(std::make_unique<Impl>(filename, options)) {}

ParquetWriter::~ParquetWriter() = default;

void ParquetWriter::Init(const Schema &schema) { impl_->Init(schema); }

void ParquetWriter::WriteColumn(int col_idx, const void *values,
                                int num_values) {
  impl_->WriteColumn(col_idx, values, num_values);
}

void ParquetWriter::Close() { impl_->Close(); }

} // namespace hpq
