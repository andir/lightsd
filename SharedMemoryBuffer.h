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
                uint8_t empty;
                RGB rgb;
            };
        };

        LargeRGB& operator=(const RGB& rhs) {
            rgb = rhs;
            return *this;
        }
    };


private:
//    MemFD memfd;
    int fd;
    MemMap memmap;
    AllocatedBuffer<LargeRGB> buffer;

public:
    SharedMemoryBuffer(const std::string filename, size_t size);
    ~SharedMemoryBuffer();
    void close();

    void resize(const size_t);


    AllocatedBuffer<LargeRGB>* get() {
        return &buffer;
    }

};


#endif //LIGHTSD_SHAREDMEMORYBUFFER_H