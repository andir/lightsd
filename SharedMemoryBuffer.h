//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_SHAREDMEMORYBUFFER_H
#define LIGHTSD_SHAREDMEMORYBUFFER_H


#include "hsv.h"
#include "buffer.h"
#include "MemFD.h"
#include "MemMap.h"

class SharedMemoryBuffer {
    MemFD memfd;
    MemMap memmap;
    AllocatedBuffer<HSV> buffer;

public:
    SharedMemoryBuffer(const std::string filename, size_t size);
    ~SharedMemoryBuffer();
    void close();


    AllocatedBuffer<HSV>* get() {
        return &buffer;
    }

};


#endif //LIGHTSD_SHAREDMEMORYBUFFER_H
