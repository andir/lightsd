//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_SHAREDMEMORYBUFFER_H
#define LIGHTSD_SHAREDMEMORYBUFFER_H


#include "rgb.h"
#include "buffer.h"
//#include "MemFD.h"
#include "MemMap.h"

class SharedMemoryBuffer {
public:
    struct LargeRGB {
        union {
            uint32_t _allocator;
            struct {
                uint8_t g;
                uint8_t r;
                uint8_t b;
                uint8_t empty;
            };
        };

        inline LargeRGB& operator=(const RGB& rhs) {
            int a = rhs.green << 24;
            int b = rhs.red   << 16;
            int c = rhs.blue  <<  8;

            _allocator = a + b + c;
            return *this;
        }
    };


private:
//    MemFD memfd;
    int fd;
    MemMap memmap;
    AllocatedBuffer<LargeRGB> buffer;

    void _resize(const size_t size);

public:
    SharedMemoryBuffer(const std::string filename, size_t size);
    ~SharedMemoryBuffer();
    void close();

    inline void ensureSize(const size_t size) {
        if (size > buffer.size()) {
            _resize(size);
        }
    }


    AllocatedBuffer<LargeRGB>* get() {
        return &buffer;
    }

};


#endif //LIGHTSD_SHAREDMEMORYBUFFER_H