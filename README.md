# FlatLSM
## 1 Introduction
A DRAM-NVM-SSD key-value store based on the LSM-tree.


## 2 Compilation and Run
### 2.1 Tools
FlatLSM acesses NVM via [PMDK](https://github.com/pmem/pmdk) . To run FlatLSM, please install PMDK first.

### 2.2 Configuration

To run FlatLSM, please modify the configuration in ``memory/nvm_module.cc``.
```
const char * PM_FILE_NAME="/mnt/pmemdir/pm_log";
```


### 2.3 Compilation
We only support Makefile instead of cmake currently.
```
> make -j64   
```


### 2.4 Run
We use db_bench for testing.
```
./db_bench ----benchmarks= "fillrandom,stats,readrandom,stats" --dbname=/mnt/data_02/dbbench --max_background_flushes=5 --max_background_compactions=1 --max_bytes_for_level_base=8589934592
```

