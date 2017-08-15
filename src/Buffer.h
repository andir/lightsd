#pragma once

#include <cassert>
#include <cstdlib>
#include <memory>
#include <iostream>

template<typename BufferType>
class Iterator {
public:
    using value_type = typename BufferType::value_type;
    using this_type = Iterator<BufferType>;

protected:
    const BufferType *const buffer;
    size_t pos;
    bool end;


public:
    Iterator(const BufferType *const buffer, size_t pos, bool end = false) : buffer(buffer),
                                                                             pos(pos),
                                                                             end(end) {
    }

    Iterator(this_type &&other) :
            buffer(other.buffer),
            pos(other.pos) {

    }

    Iterator(const this_type &other) :
            buffer(other.buffer),
            pos(other.pos) {

    }

    Iterator(this_type &other) :
            buffer(other.buffer),
            pos(other.pos) {

    }

    Iterator &operator=(const Iterator &) = default;

    Iterator operator=(const Iterator &&other) {
        return Iterator(other.buffer, other.pos);
    }

    bool operator==(const this_type &other) const {
        return pos == other.pos;
    }

    bool operator!=(const this_type &other) const {
        if (other.end) {
            return (pos < other.pos);
        }

        return pos != other.pos;
    }

    bool operator<(const this_type &other) const {
        return pos < other.pos;
    }

    bool operator>(const this_type &other) const {
        return pos > other.pos;
    }

    inline value_type &operator*() const {
        return buffer->at(pos);
    }

    inline const this_type &operator+(const size_t offset) {
        pos += offset;
        return *this;
    }

    inline const this_type &operator++(int) {
        ++pos;
        return *this;
    }

    inline const this_type &operator++() {
        ++pos;
        return *this;
    }

    inline const size_t operator-(const this_type &other) {
        return pos - other.pos;
    }

};

template<typename BufferType>
class ReverseIterator : public Iterator<BufferType> {
public:
    using this_type = ReverseIterator<BufferType>;
    using Iterator<BufferType>::Iterator;


    inline const this_type &operator+(const size_t offset) {
        this->pos -= offset;
        return *this;
    }

    inline const this_type &operator++(int) {
        --this->pos;
        return *this;
    }

    inline const this_type &operator++() {
        --this->pos;
        return *this;
    }
};


template<typename ValueType>
class AbstractBaseBuffer {
public:
    using value_type = ValueType;
    using value_array_type = value_type[];
    using abstract_base_buffer_type = AbstractBaseBuffer<ValueType>;
    using IteratorType = Iterator<AbstractBaseBuffer>;
    using ReverseIteratorType = ReverseIterator<AbstractBaseBuffer>;


    virtual value_type &at(size_t pos) const = 0;

    virtual size_t size() const = 0;

    virtual size_t count() const = 0;

    IteratorType begin() const {
        return IteratorType(this, 0);
    }

    ReverseIteratorType rbegin() const {
        return ReverseIteratorType(this, count());
    }

    IteratorType end() const {
        return IteratorType(this, count(), true);
    }

    ReverseIteratorType rend() const {
        return ReverseIteratorType(this, 0);
    }

};

template<typename ValueType>
class AllocatedBuffer : public AbstractBaseBuffer<ValueType> {
public:
    using parent = AbstractBaseBuffer<ValueType>;
    using value_type = typename parent::value_type;
    using value_array_type = typename parent::value_array_type;
    using ptr_type = typename std::unique_ptr<value_array_type>;
    using IteratorType = Iterator<AllocatedBuffer>;

private:
    ptr_type ptr;

public:
    size_t _count;

    AllocatedBuffer(const size_t count, typename parent::value_type *ptr)
            :
            ptr(ptr), _count(count) {
        assert(ptr != nullptr || (ptr == nullptr && count == 0));
    }

    AllocatedBuffer(const size_t count) : ptr(std::make_unique<value_array_type>(count)), _count(count) {
        // allocate memory and in case we've to exit this function early, release it.
        assert(ptr != nullptr || count == 0);

        std::cerr << "Constructed AllocatedBuffer. data begin: " << (void *) &(*ptr.get()) << std::endl;

    }

    // disable copyconstructor from const object
    AllocatedBuffer(const AllocatedBuffer &other) = delete;

    // disable copy from non-const object
    AllocatedBuffer(AllocatedBuffer &other) = delete;

    // enable move construction (e.g. function return value)
    AllocatedBuffer(AllocatedBuffer &&other) :
            ptr(other.ptr.release()),
            _count(other._count) {
        std::cerr << "Copy-constructed AllocatedBuffer. data begin: " << &(*ptr.get()) << std::endl;
    }

    AllocatedBuffer &operator=(AllocatedBuffer &&rhs) = default;


    virtual ~AllocatedBuffer() {
        std::cerr << "Destructing buffer" << std::endl;
    }

    inline typename parent::value_type &at(size_t pos) const {
        assert(pos < _count);
        return ptr.get()[pos];
    }

    size_t size() const {
        return _count;
    }

    size_t count() const {
        return _count;
    }

    inline IteratorType begin() const {
        return IteratorType(this, 0);
    }

    inline IteratorType end() const {
        return IteratorType(this, count(), true);
    }

    auto release() {
        _count = 0;
        auto v = ptr.release();
        return v;
    }

};

template<typename ValueType>
class BufferView : public AbstractBaseBuffer<typename ValueType::value_type> {
public:
    using parent = AbstractBaseBuffer<typename ValueType::value_type>;
    using value_type = typename parent::value_type;
    using IteratorType = Iterator<BufferView>;
    using UpperT = std::shared_ptr<AbstractBaseBuffer<typename ValueType::value_type> >;
private:
    UpperT upper;
    size_t offset;
    size_t len;

public:
    size_t _count;

    BufferView(UpperT& upper, size_t offset, size_t len) : upper(upper), offset(offset), _count(len) {
        
    }

    inline typename parent::value_type &at(size_t pos) const {
       assert(pos < _count);
       const size_t i = offset + pos;
       return upper->at(i); 
    }

    size_t size() const {
        return _count;
    }

    size_t count() const {
        return _count;
    }

    inline IteratorType begin() const {
        return IteratorType(this, 0);
    }

    inline IteratorType end() const {
        return IteratorType(this, _count, true);
    }
   
};


