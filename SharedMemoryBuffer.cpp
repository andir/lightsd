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
    assert(ftruncate(fd, size * sizeof(RGB)) == 0);
    assert(memmap.open(fd, size * sizeof(RGB)));

//    assert(memfd.open(MFD_ALLOW_SEALING));
//    assert(memfd.setSize(size));
//    assert(memfd.seal());

    auto ptr = memmap.get<RGB>();
    buffer = AllocatedBuffer<RGB>(size, ptr);
}

void SharedMemoryBuffer::resize(const size_t size) {
    if (size > buffer.size()) {
        buffer.release();
        memmap.close();
        assert(ftruncate(fd, size * sizeof(RGB)) == 0);
        memmap.open(fd, size * sizeof(RGB));
        auto ptr = memmap.get<RGB>();
        buffer = AllocatedBuffer<RGB>(size, ptr);
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