//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once

#include "db/compaction/compaction_picker.h"

namespace ROCKSDB_NAMESPACE {
// Picking compactions for leveled compaction. See wiki page
// https://github.com/facebook/rocksdb/wiki/Leveled-Compaction
// for description of Leveled compaction.
class L0CompactionPicker : public CompactionPicker {
public:
    L0CompactionPicker(const ImmutableOptions& ioptions,
                          const InternalKeyComparator* icmp)
            : CompactionPicker(ioptions, icmp) {}
    Compaction* PickCompaction(const std::string& cf_name,
                               const MutableCFOptions& mutable_cf_options,
                               const MutableDBOptions& mutable_db_options,
                               VersionStorageInfo* vstorage,
                               LogBuffer* log_buffer) override;
  CompactionL0* PickCompactionL0(const std::string& cf_name,
                             const MutableCFOptions& mutable_cf_options,
                             const MutableDBOptions& mutable_db_options,
                             VersionStorageInfo* vstorage,
                             LogBuffer* log_buffer,
                             std::list<MemTable*>&mem_list) override;

    bool NeedsCompaction(const VersionStorageInfo* vstorage) const override;
};

}  // namespace ROCKSDB_NAMESPACE
