#pragma once

#include <iostream>
#include <vector>

struct RGB;

struct HSV {
  using value_type = float;

  value_type hue;
  value_type saturation;
  value_type value;

  RGB toRGB() const;
  inline HSV toHSV() const { return *this; }

  HSV operator+=(const HSV& rhs);

  HSV operator-(const HSV& rhs) const;

  HSV operator/(const int div) const {
    return HSV{
        hue / value_type(div),
        saturation / value_type(div),
        value / value_type(div),
    };
  }

  HSV operator*(const float mul) const;

  HSV operator+(const HSV& rhs) const;
  HSV operator=(const RGB& rhs);

  void print() const;

  std::string string() const;
};

std::ostream& operator<<(std::ostream& os, const HSV t);
