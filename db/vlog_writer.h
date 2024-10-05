#pragma once


#include <cstdint>
#include <memory>

#include "rocksdb/slice.h"


namespace rocksdb {

class WritableFile;


namespace vlog {
class VlogInfo;
class VlogManager;
class VWriter {
 public:
  // Create a writer that will append data to "*dest".
  // "*dest" must be initially empty.
  // "*dest" must remain live while this Writer is in use.
  explicit VWriter(WritableFile* dest);

  ~VWriter();

  uint64_t AddRecord(const Slice& slice);
  void Close();
  uint64_t FileSize() const { return size_; }
  friend class VlogManager;

 private:
  VWriter() = default;

  VlogInfo* my_info_;
  std::unique_ptr<WritableFile> dest_;
  // No copying allowed
  VWriter(const VWriter&);
  void operator=(const VWriter&);
  uint64_t  size_;
};

}  // namespace vlog
}  // namespace leveldb


