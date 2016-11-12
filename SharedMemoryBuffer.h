//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_SHAREDMEMORYBUFFER_H
#define LIGHTSD_SHAREDMEMORYBUFFER_H


#include "rgb.h"
#include "buffer.h"
#include "MemFD.h"
#include "MemMap.h"

class SharedMemoryBuffer {
//    MemFD memfd;
    int fd;
    MemMap memmap;
    AllocatedBuffer<RGB> buffer;

public:
    SharedMemoryBuffer(const std::string filename, size_t size);
    ~SharedMemoryBuffer();
    void close();

    void resize(const size_t);

    AllocatedBuffer<RGB>* get() {
        return &buffer;
    }

};


#endif //LIGHTSD_SHAREDMEMORYBUFFER_H
