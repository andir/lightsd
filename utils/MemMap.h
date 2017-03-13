//
// Created by andi on 10/3/16.
//

#ifndef LIGHTSD_MEMMAP_H
#define LIGHTSD_MEMMAP_H


class MemMap {
    size_t size;
    void *ptr;
public:
    MemMap();

    bool open(const int fd, const size_t size);

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


#endif //LIGHTSD_MEMMAP_H
