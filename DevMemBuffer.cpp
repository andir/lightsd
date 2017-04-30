//
// Created by andi on 10/3/16.
//

#include "DevMemBuffer.h"
#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

DevMemBuffer::DevMemBuffer(const std::string filename, size_t size, size_t count_offset, size_t data_offset) :
        count_offset(count_offset), data_offset(data_offset),
        buffer(0, nullptr) {


    fd = open(filename.c_str(), O_RDWR);
    assert(fd >= 0);

    assert(memmapCount.open(fd, sizeof(uint32_t), count_offset));
    assert(memmapData.open(fd, sizeof(LargeRGB) * size, data_offset));
    
    auto ptr = memmapData.get<LargeRGB>();
    buffer = AllocatedBuffer<LargeRGB>(size, ptr);
}

void DevMemBuffer::close() {
    if (fd >= 0)
        ::close(fd);
    buffer.release();
    memmapData.close();
    memmapCount.close();
}

DevMemBuffer::~DevMemBuffer() {
    close();
}
void DevMemBuffer::_resize(const size_t size) {
       // update data buffer
       buffer.release();
       memmapData.close();
       assert(memmapData.open(fd, size * sizeof(LargeRGB), data_offset));
       auto bufferPtr = memmapData.get<LargeRGB>();
        
       // update count buffer
       auto ptr = memmapCount.get<uint32_t>();
       ptr[0] = size;

       buffer = AllocatedBuffer<LargeRGB>(size, bufferPtr);

}
