//
// Created by andi on 10/3/16.
//

#include "SharedMemoryBuffer.h"
SharedMemoryBuffer::SharedMemoryBuffer(const std::string filename, size_t size) :
        memfd(filename), buffer(0, nullptr) {
    assert(memfd.open(MFD_ALLOW_SEALING));
    assert(memfd.setSize(size));
    assert(memfd.seal());
    assert(memmap.open(memfd.fd, size));

    auto ptr = memmap.get<HSV>();
    buffer = AllocatedBuffer<HSV>(size / sizeof(HSV), ptr);
}

void SharedMemoryBuffer::close() {
    buffer.release();
    memmap.close();
    memfd.close();
}

SharedMemoryBuffer::~SharedMemoryBuffer() {
    close();
}