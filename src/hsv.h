#pragma once

#include <array>
#include <iostream>
#include <vector>

struct RGB;

struct HSV {
  using value_type = float;

  value_type hue;
  value_type saturation;
  value_type value;

  RGB toRGB() const;

  HSV operator+=(const HSV& rhs);

  HSV operator-(const HSV& rhs) const;

  HSV operator/(int div) const {
    return HSV{
        hue / value_type(div), saturation / value_type(div),
        value / value_type(div),
    };
  }

  HSV operator+(const HSV& rhs) const;
  HSV operator=(const RGB& rhs);

  void print() const;

  std::string string() const;
};
