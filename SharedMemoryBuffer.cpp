//
// Created by andi on 10/3/16.
//

#include "SharedMemoryBuffer.h"
#include <iostream>

int _shm_open(const std::string filename) {
    int fd = shm_open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);

    return fd;
}

SharedMemoryBuffer::SharedMemoryBuffer(const std::string filename, size_t size) :
//        memfd(filename),
    buffer(0, nullptr) {

    fd = _shm_open(filename);

    std::cerr << filename << std::endl;

    assert(fd >= 0);
    assert(ftruncate(fd, size * sizeof(LargeRGB)) == 0);
    assert(memmap.open(fd, size * sizeof(LargeRGB)));


    auto ptr = memmap.get<LargeRGB>();
    buffer = AllocatedBuffer<LargeRGB>(size, ptr);
}

void SharedMemoryBuffer::_resize(const size_t size) {
    if (size > buffer.size()) {
        buffer.release();
        memmap.close();
        assert(ftruncate(fd, size * sizeof(LargeRGB)) == 0);
        memmap.open(fd, size * sizeof(LargeRGB));
        auto ptr = memmap.get<LargeRGB>();
        buffer = AllocatedBuffer<LargeRGB>(size, ptr);
    }
}

void SharedMemoryBuffer::close() {
    if (fd >= 0)
        ::close(fd);
    buffer.release();
    memmap.close();

}

SharedMemoryBuffer::~SharedMemoryBuffer() {
    close();
}