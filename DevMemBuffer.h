#pragma once
#include "rgb.h"
#include "Buffer.h"
#include "utils/MemMap.h"

class DevMemBuffer {
//    MemFD memfd;
    int fd;
    size_t count_offset;
    size_t data_offset;
    MemMap memmapCount;
    MemMap memmapData;

    AllocatedBuffer<RGB> buffer;

    void _resize(const size_t size);

public:
    DevMemBuffer(const std::string filename = "/dev/mem", size_t size = 16000, size_t count_offset = 0x40000000, size_t data_offset = 0x18000000);

    ~DevMemBuffer();

    void close();

    inline void ensureSize(const size_t size) {
        if (size != buffer.size()) {
                _resize(size);
        }
    }


    AllocatedBuffer<RGB> *get() {
        return &buffer;
    }

};

