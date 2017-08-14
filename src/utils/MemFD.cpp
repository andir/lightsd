//
// Created by andi on 10/3/16.
//

#include <cassert>
#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>
#include "MemFD.h"

namespace {
    static inline int memfd_create(const char *name, unsigned int flags) {
        return syscall(__NR_memfd_create, name, flags);
    }
}


#ifndef F_LINUX_SPECIFIC_BASE
#define F_LINUX_SPECIFIC_BASE 1024
#endif

#ifndef F_ADD_SEALS
#define F_ADD_SEALS (F_LINUX_SPECIFIC_BASE + 9)
#define F_GET_SEALS (F_LINUX_SPECIFIC_BASE + 10)

#define F_SEAL_SEAL     0x0001  /* prevent further seals from being set */
#define F_SEAL_SHRINK   0x0002  /* prevent file from shrinking */
#define F_SEAL_GROW     0x0004  /* prevent file from growing */
#define F_SEAL_WRITE    0x0008  /* prevent writes */
#endif


MemFD::MemFD(const std::string& filename) : filename(filename), fd(0) {
}

MemFD::~MemFD() {
    close();
}


void MemFD::close() {
    if (fd > 0) {
        ::close(fd);
        fd = 0;
    }
}

bool MemFD::open(unsigned int flags) {
    int rc = memfd_create(filename.c_str(), flags);
    if (rc == -1)
        return false;
    fd = rc;

    return true;
}

bool MemFD::setSize(size_t size) {
    if (fd <= 0)
        return false;

    int ret = ftruncate(fd, size);

    return ret != -1;
}


bool MemFD::seal() {
    if (fd <= 0)
        return false;
    int ret = fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK | F_SEAL_GROW);

    return ret != -1;
}
