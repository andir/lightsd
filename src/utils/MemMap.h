#pragma once

class MemMap {
    size_t size;
    void *ptr;
public:
    MemMap();

    bool open(const int fd, const size_t size, const size_t offset = 0);

    void close();

    ~MemMap();

    template<typename T>
    T *get() const {
        return reinterpret_cast<T *>(ptr);
    }

    size_t getSize() const {
        return size;
    }
};
