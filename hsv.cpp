//
// Created by andi on 10/2/16.
//

#include <tgmath.h>
#include "hsv.h"
#include "rgb.h"
#include "algorithm.h"


template<typename A, int min, int max>
A ensure_bounds(A val) {

    A minA = A(min), maxA = A(max);

    return std::max(std::min(
            val, maxA
    ), minA);
};

RGB HSV::toRGB() const {
    double hh, p, q, t, ff;
    long i;

    if (saturation <= 0) {
        return RGB::make_RGB(
                this->value,
                this->value,
                this->value
        );
    }

    hh = hue;

    if (hh > 360) hh = 0.0;

    hh /= 60;

    i = (long) hh;

    ff = hh - i;


    p = value * (1.0 - saturation);
    q = value * (1.0 - (saturation * ff));
    t = value * (1.0 - (saturation * (1.0 - ff)));

    switch (i) {
        case 0:
            return RGB::make_RGB(value, t, p);
        case 1:
            return RGB::make_RGB(q, value, p);
        case 2:
            return RGB::make_RGB(p, value, t);
        case 3:
            return RGB::make_RGB(p, q, value);
        case 4:
            return RGB::make_RGB(t, p, value);
        case 5:
        default:
            return RGB::make_RGB(value, p, q);
    }

}

HSV HSV::operator+=(const HSV &rhs) {

    this->saturation += rhs.saturation;
    this->hue += rhs.hue;
    this->hue = fmod(hue, 360);
    this->value += rhs.value;

    return *this;
}

void HSV::print() const {
    std::cout << string();
}

std::string HSV::string() const {
    std::stringstream ss;
    ss << "HSV{" << hue << ", " << saturation << ", " << value << "}" ;
    return ss.str();
}


HSV HSV::operator-(const HSV &rhs) const {
    const auto hd = hue - rhs.hue;
    algorithm::cubic_add<float> m;

    auto a = value_type(fmod(hue - rhs.hue, 360));
    if (a < 0)
        a = 360 + a;
    auto b = value_type(fmod(rhs.hue - hue, 360));
    if (b < 0)
        b = 360 + b;

    return HSV{
            a < b ? a : b,
            ensure_bounds<value_type , -1, 1>(saturation - rhs.saturation),
            ensure_bounds<value_type , -1, 1>(value - rhs.value),
    };
}

HSV HSV::operator+(const HSV &rhs) const {
    algorithm::cubic_add<float> m;
    return HSV{
            std::max(value_type(fmod(hue + rhs.hue, 360)), 0.0f),
            //std::min(std::max(, 0.0f), 360.0f),
            ensure_bounds<value_type , 0, 1>(saturation + rhs.saturation),
            ensure_bounds<value_type , 0, 1>(value + rhs.value),
    };
}