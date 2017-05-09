#pragma once

#include <vector>
#include "rgb.h"
#include "Buffer.h"
#include "utils/MemMap.h"

class DevMemBuffer {
public:


    struct FPGASettings {
        uint32_t t0l;
        uint32_t t0h;
        uint32_t t1l;
        uint32_t t1h;
        uint32_t res;
        uint32_t read_addr;
        uint32_t led_count[50];
    };

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

    int fd;
    std::vector<size_t> sizes;
    size_t count_offset;
    size_t data_offset;
    MemMap memmapConfig;
    MemMap memmapData;

    AllocatedBuffer<LargeRGB> buffer;

    void _resize(const std::vector<size_t> sizes);
public:
    DevMemBuffer(const std::string filename = "/dev/mem", std::vector<size_t> sizes = {16000}, size_t configuration_offset = 0x40000000, size_t data_offset = 0x1ff00000);
    ~DevMemBuffer();

    void close();

    AllocatedBuffer<LargeRGB> *get() {
        return &buffer;
    }
    void writeConfiguration();
};

