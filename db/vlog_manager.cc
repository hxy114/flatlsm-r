
#include "db/vlog_manager.h"

#include <iostream>

#include "db/vlog_fetcher.h"
#include "db/vlog_writer.h"
#include "file/filename.h"
#include "util/coding.h"

namespace rocksdb {
namespace vlog {



VlogManager::~VlogManager() {
  for (auto& it : manager_) {
    /*if (it.first == cur_vlog_) {
      it.second->vlog_write_->dest_->Append(
          Slice(it.second->buffer_, it.second->size_));
    }*/
    delete it.second->vlog_fetch_;
    delete it.second->vlog_write_;
    delete it.second;
  }
}

VWriter* VlogManager::AddVlog(const std::string& log_file_name, const ImmutableOptions* options,uint64_t vlog_numb) {
  /*VlogInfo* old = manager_[vlog_numb];
  if (old != nullptr) {
    old->vlog_write_->dest_->Append(Slice(old->buffer_, old->size_));
  }*/
  //std::lock_guard<std::mutex> lock(mutex);
  VlogInfo* v = new VlogInfo;
  v->vlog_write_ = new VWriter;
  v->vlog_write_->size_=0;
  //std::cout<<"vlog_numb"<<vlog_numb<<std::endl;
  Status s = options->env->NewWritableFile(log_file_name,
                                            &v->vlog_write_->dest_,EnvOptions());
  assert(s.ok());
  // VlogFetcher must initialize after WritableFile is created;
  v->vlog_fetch_ = new VlogFetcher(log_file_name, options);
  v->vlog_write_->my_info_ = v;
  v->vlog_fetch_->my_info_ = v;
  v->count_ = 0;

  mutex_.lock();
  manager_[vlog_numb] = v;
  mutex_.unlock();



  //cur_vlog_ = vlog_numb;
  return v->vlog_write_;
}

//void VlogManager::SetCurrentVlog(uint64_t vlog_numb) { cur_vlog_ = vlog_numb; }

Status VlogManager::FetchValueFromVlog(Slice addr, std::string* value) {
  Status s;
  uint64_t file_numb, offset, size;
  //std::cout<<"addr.size"<<addr.size()<<std::endl;
  // address is <vlog_number, vlog_offset, size>
  if (!GetVarint64(&addr, &file_numb))
    return Status::Corruption("parse size false in RealValue");
  if (!GetVarint64(&addr, &offset))
    return Status::Corruption("parse file_numb false in RealValue");
  if (!GetVarint64(&addr, &size))
    return Status::Corruption("parse pos false in RealValue");
  //std::cout<<"file_numb"<<file_numb<<std::endl;
  //std::cout<<"offset"<<offset<<std::endl;
  //std::cout<<"size"<<size<<std::endl;
  //assert(offset%4116==0);
  //assert(size==4116);
  std::unordered_map<uint64_t, VlogInfo*>::const_iterator iter = manager_.find(file_numb);
  if (iter == manager_.end() || iter->second->vlog_fetch_ == nullptr) {
    s = Status::Corruption("can not find vlog");
  } else {
    VlogFetcher* cache = iter->second->vlog_fetch_;
    s = cache->Get(offset, size, value);
  }

  return s;
}
Status VlogManager::AddRecord(uint64_t file_numb,const Slice& slice) {
  std::unordered_map<uint64_t, VlogInfo*>::const_iterator iter = manager_.find(file_numb);
  assert(iter != manager_.end());
  assert(iter->second != nullptr);
  return iter->second->vlog_write_->AddRecord(slice)!=UINT64_MAX ? Status::OK():Status::IOError("append error");
}
Status VlogManager::Sync(uint64_t file_numb) {
  std::unordered_map<uint64_t, VlogInfo*>::const_iterator iter = manager_.find(file_numb);
  assert(iter != manager_.end());
  assert(iter->second != nullptr);
  return iter->second->vlog_write_->dest_->Sync();
}

/*Status VlogManager::SetHead(size_t offset) {
  std::map<uint64_t, VlogInfo*>::const_iterator iter = manager_.find(cur_vlog_);
  if (iter == manager_.end() || iter->second->vlog_fetch_ == nullptr) {
    return Status::Corruption("can not find vlog");
  } else {
    iter->second->head_ = offset;
    return Status::OK();
  }
}*/

}  // namespace vlog
}  // namespace leveldb
