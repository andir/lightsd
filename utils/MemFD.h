//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_MEMFD_H
#define LIGHTSD_MEMFD_H

#include <string>
#include <linux/memfd.h>

enum class MemFDFlags : int {
    CLOEXEC = MFD_CLOEXEC,
    ALLOW_SEALING = MFD_ALLOW_SEALING,
};


class MemFD {
public:
    const std::string filename;
    int fd;

    explicit MemFD(const std::string& filename);

    bool open(unsigned int flags);

    bool setSize(size_t size);

    bool seal();

    void close();

    ~MemFD();
};


#endif //LIGHTSD_MEMFD_H
