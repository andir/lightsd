//
// Created by andi on 10/2/16.
//

#ifndef LIGHTSD_HSV_H
#define LIGHTSD_HSV_H

#include <vector>
#include <array>
#include <iostream>

struct RGB;


struct HSV {

    using value_type = float;


    value_type hue;
    value_type saturation;
    value_type value;

    RGB toRGB() const;

    HSV operator+=(const HSV &rhs);

    HSV operator-(const HSV &rhs) const;

    HSV operator/(int div) const {
        return HSV{
                hue / value_type(div),
                saturation / value_type(div),
                value / value_type(div),
        };
    }

    HSV operator+(const HSV &rhs) const;
    HSV operator=(const RGB &rhs) const;

    void print() const;

    std::string string() const;

};

#endif //LIGHTSD_HSV_H
