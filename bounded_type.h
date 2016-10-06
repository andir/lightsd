//
// Created by andi on 10/5/16.
//

#ifndef LIGHTSD_BOUNDED_TYPE_H
#define LIGHTSD_BOUNDED_TYPE_H


template<typename T, int Min, int Max>
struct min_max_constrained {
    static inline T apply_bounds(const T v) {
        if (v < Min) {
            return T(Min);
        } else if (v > Max) {
            return T(Max);
        } else {
            return v;
        }
    }
};

template<typename T, int Min, int Max, typename constraint = min_max_constrained<T, Min, Max>>
struct ConstrainedType {
    T value;

    using SelfType = ConstrainedType<T, Min, Max>;

    static inline T apply_bounds(const T v) {
        return constraint::apply_bounds(v);
    }

public:

    explicit constexpr ConstrainedType(T v) : value(apply_bounds(v)) {
//        static_assert(v >= Min, "Must be >= Min");
        //static_assert(v <= Max, "Must be <= Max");
    }

    SelfType operator++(int) {
        T val = value;
        value = apply_bounds(value+1);
        return SelfType(this);
    }

    SelfType operator++() {
        value = apply_bounds(value+1);
        return *this;
    }

    SelfType operator--(int) {
        T val = value;
        value = apply_bounds(value-1);
        return SelfType(val);
    }

    SelfType operator--() {
        value = apply_bounds(value-1);
        return *this;
    }

    SelfType& operator+=(const T& v) {
        value = apply_bounds(value + v);
        return *this;
    }

    SelfType& operator-=(const T& v) {
        value = apply_bounds(value - v);
        return *this;
    }

    SelfType& operator*=(const T& v) {
        value = apply_bounds(value * v);
        return *this;
    }

    SelfType& operator/=(const T& v) {
        value = apply_bounds(value / v);
        return *this;
    }

    SelfType& operator%=(const T& v) {
        value = apply_bounds(value % v);
        return *this;
    }

    SelfType& operator^=(const T& v) {
        value = apply_bounds(value ^ v);
        return *this;
    }

    SelfType& operator&=(const T& v) {
        value = apply_bounds(value & v);
        return *this;
    }

    SelfType& operator|=(const T& v) {
        value = apply_bounds(value | v);
        return *this;
    }

    SelfType& operator>>=(const T& v) {
        value = apply_bounds(value >> v);
        return *this;
    }

    SelfType& operator<<=(const T& v) {
        value = apply_bounds(value << v);
        return *this;
    }

    template<typename O>
    bool operator>(const O o) const {
        return value > o;
    }

    template<typename O>
    bool operator<(const O o) const {
        return value < o;
    }

    template<typename O>
    bool operator>=(const O o) const {
        return value >= o;
    }

    template<typename O>
    bool operator<=(const O o) const {
        return value <= o;
    }

    template<typename O>
    bool operator==(const O o) const {
        return value == o;
    }

};


#endif //LIGHTSD_BOUNDED_TYPE_H
