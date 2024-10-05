#pragma once

#include <atomic>
#include <map>
#include <set>

#include "monitoring/instrumented_mutex.h"
#include "options/cf_options.h"
#include "rocksdb/options.h"
#include "rocksdb/status.h"

namespace rocksdb {
namespace vlog {
// Header is checksum (4 bytes), length (8 bytes).
static const int kVHeaderSize = 4 + 8;

static const int WriteBufferSize = 1 << 12;

class VlogFetcher;
class VWriter;

class VlogInfo {
  //char buffer_[WriteBufferSize];
  //size_t size_;
  VlogFetcher* vlog_fetch_;
  VWriter* vlog_write_;
  //size_t head_;

  uint64_t count_;  //代表该vlog文件垃圾kv的数量

  //port::SharedMutex* rwlock_;

 public:
  VlogInfo() =default;
  ~VlogInfo()=default;

  friend class VWriter;
  friend class VlogFetcher;
  friend class VlogManager;
};

class VlogManager {
 public:
  VlogManager()=default;
  ~VlogManager();

  VWriter* AddVlog(const std::string& dbname, const ImmutableOptions* options,uint64_t vlog_numb);

  Status AddRecord(uint64_t file_numb,const Slice& slice);

  //Status SetHead(size_t offset);

  Status Sync(uint64_t file_numb);

  Status FetchValueFromVlog(Slice addr, std::string* value);

  //void SetCurrentVlog(uint64_t vlog_numb);

 private:
  std::mutex mutex_;
  std::unordered_map<uint64_t, VlogInfo*> manager_;
  //std::set<uint64_t> cleaning_vlog_set_;
  uint64_t clean_threshold_;
  //uint64_t cur_vlog_;

};

}  // namespace vlog
}  // namespace leveldb


