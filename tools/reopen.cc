//
// Created by hxy on 24-5-23.
//
#include <iostream>
#include <cstdio>
#include <string>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

rocksdb::Env* g_env = nullptr;

int main() {
  rocksdb::DB* db_= nullptr;
  g_env = rocksdb::Env::Default();
  assert(db_ == nullptr);
  rocksdb::Options options;
  options.env = g_env;
  std::string path = "/tmp/rocksdbtest-1000/dbbench/";
  const uint64_t start_micros = g_env->NowMicros();
  rocksdb::Status s = rocksdb::DB::Open(options, path, &db_);
  std::cout << g_env->NowMicros() - start_micros<<"  "<<s.ToString();

  delete db_;


}