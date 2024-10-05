//
// Created by hxy on 24-3-29.
//
#include "memory/nvm_arena.h"

namespace rocksdb {

static const int kBlockSize = 4096;

NvmArena::NvmArena(PmLogHead *pm_log_start,AllocTracker* tracker,bool force,bool recover)
    : pm_log_start_(pm_log_start), force_(force), memory_usage_(PM_LOG_HEAD_SIZE),
      kv_start_((char*)pm_log_start_+PM_LOG_HEAD_SIZE ),kv_alloc_ptr_(kv_start_),last_persist_point_(kv_start_), tracker_(tracker){
  if(recover){
    memory_usage_=pm_log_start_->used_size;
    kv_alloc_ptr_=(char*)pm_log_start_+memory_usage_;
    last_persist_point_=kv_alloc_ptr_;
  }
}
NvmArena::NvmArena() {}
NvmArena::~NvmArena() {
  //TODO maybe归还pmlog
  Persist();
  if (tracker_ != nullptr) {
    assert(tracker_->is_freed());
    tracker_->FreeMem();
  }
}

char* NvmArena::Allocate(size_t bytes) {
  if(kv_alloc_ptr_-last_persist_point_>static_cast<long long int>(PERSIST_SIZE)){
    Persist();
  }

  char *result=kv_alloc_ptr_;
  kv_alloc_ptr_+=bytes;
  memory_usage_+=bytes;
  if (tracker_ != nullptr) {
    tracker_->Allocate(bytes);
  }
  return result;
}

char* NvmArena::AllocateAligned(size_t bytes, size_t /*huge_page_size*/,
            Logger* /*logger*/) {
  if(kv_alloc_ptr_-last_persist_point_ > static_cast<long long int>(PERSIST_SIZE)){
    Persist();
  }
  const int align = (sizeof(void*) > 8) ? sizeof(void*) : 8;
  static_assert((align & (align - 1)) == 0,
                "Pointer size should be a power of 2");
  size_t current_mod = reinterpret_cast<uintptr_t>(kv_alloc_ptr_) & (align - 1);
  size_t slop = (current_mod == 0 ? 0 : align - current_mod);
  size_t needed = bytes + slop;
  char* result;

  result = kv_alloc_ptr_ + slop;
  kv_alloc_ptr_ += needed;
  memory_usage_+=needed;
  if (tracker_ != nullptr) {
    tracker_->Allocate(bytes);
  }
  assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
  return result;
}

size_t NvmArena::BlockSize() const { return kBlockSize; }
void NvmArena::Persist(){
  if(force_||kv_alloc_ptr_-last_persist_point_>static_cast<long long int>(PERSIST_SIZE)){
    PersistKV();
    PersistHead();
    last_persist_point_=kv_alloc_ptr_;
  }
}
void NvmArena::PersistKV(){
  pmem_persist(last_persist_point_,kv_alloc_ptr_-last_persist_point_);


}
void NvmArena::PersistHead() {
  pm_log_start_->used_size=memory_usage_;
  pmem_persist(&(pm_log_start_->used_size),sizeof(pm_log_start_->used_size));//持久化使用量

  //pmem_persist(head_start,PM_LOG_HEAD_SIZE);

}


}  // namespace leveldb