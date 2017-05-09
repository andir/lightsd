//
// Created by andi on 10/3/16.
//

#include "DevMemBuffer.h"
#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <numeric>

DevMemBuffer::DevMemBuffer(const std::string filename, std::vector<size_t> sizes, size_t configuration_offset,
                           size_t data_offset) :
        sizes(sizes),
        count_offset(configuration_offset),
        data_offset(data_offset),
        buffer(0, nullptr) {

    fd = open(filename.c_str(), O_RDWR);
    assert(fd >= 0);

    assert(memmapConfig.open(fd, sizeof(FPGASettings), configuration_offset));
    _resize(sizes);
}

void DevMemBuffer::close() {
    if (fd >= 0)
        ::close(fd);
    buffer.release();
    memmapData.close();
    memmapConfig.close();
}

DevMemBuffer::~DevMemBuffer() {
    close();
}

void DevMemBuffer::_resize(const std::vector<size_t> sizes) {
    // update data buffer
    buffer.release();
    memmapData.close();

    const auto sum = std::accumulate(sizes.begin(), sizes.end(), 0);

    assert(memmapData.open(fd, sum * sizeof(LargeRGB), data_offset));
    auto bufferPtr = memmapData.get<LargeRGB>();

    // update count buffer
    buffer = AllocatedBuffer<LargeRGB>(sum, bufferPtr);

    writeConfiguration();
}

void DevMemBuffer::writeConfiguration() {
    auto ptr = memmapConfig.get<FPGASettings>();

    assert(ptr != nullptr);

    size_t i = 0;
    for (const auto &e : sizes) {
        ptr->led_count[i++] = e;
    }

    ptr->read_addr = (uint32_t) data_offset;
}
