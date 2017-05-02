#pragma once
#include "rgb.h"
#include "Buffer.h"
#include "utils/MemMap.h"

class DevMemBuffer {
public:
    struct LargeRGB {
        union {
            uint32_t _allocator;
            struct {
                uint8_t b;
                uint8_t g;
                uint8_t r;
                uint8_t empty;
            };
        };

        inline LargeRGB &operator=(const RGB &rhs) {
                r = rhs.red;
                g = rhs.green;
                b = rhs.blue;
                empty = 0;
                return *this;
        }
    };


//    MemFD memfd;
    int fd;
    size_t size;
    size_t count_offset;
    size_t data_offset;
    MemMap memmapCount;
    MemMap memmapData;

    AllocatedBuffer<LargeRGB> buffer;

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


    AllocatedBuffer<LargeRGB> *get() {
        return &buffer;
    }
    void writeSize();  
};

