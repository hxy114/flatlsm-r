#pragma once

#include <rocksdb/env.h>
#include <util/mutexlock.h>

#include <cstdint>
#include <list>
#include <map>
#include <string>

#include "options/cf_options.h"
#include "port/port.h"
#include "rocksdb/cache.h"
#include "rocksdb/table.h"

namespace rocksdb {
namespace vlog {

class VlogInfo;
class VlogManager;

class VlogFetcher {
 public:
  VlogFetcher(const std::string& log_file_name, const ImmutableOptions* options);

  ~VlogFetcher();

  Status Get(uint64_t offset, uint64_t size, std::string* value);

  friend class VlogManager;

 private:
  VlogInfo* my_info_;

  std::unique_ptr<RandomAccessFile> file_;
};
}  // namespace vlog
}  // namespace leveldb
