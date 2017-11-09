//
// Created by andi on 10/3/16.
//

#include "MemMap.h"
#include <cassert>
#include <cstdio>
#include <sys/mman.h>

MemMap::MemMap() : size(0), ptr(0) {}

bool MemMap::open(const int fd, const size_t size, const size_t offset) {
  if (this->size == 0) {
    this->size = size;
    void* shm =
        mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    assert(shm != MAP_FAILED);
    ptr = shm;
    return true;
  } else {
    return false;
  }
}

void MemMap::close() {
  if (ptr != nullptr) {
    int ret = munmap(ptr, size);
    assert(ret != -1);
    ptr = nullptr;
    size = 0;
  }
}

MemMap::~MemMap() {
  close();
}
