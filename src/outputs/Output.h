#pragma once

#include <cmath>
#include "../Buffer.h"
#include "../hsv.h"

class Output {
 public:
  virtual ~Output(){};

  virtual void draw(const AbstractBaseBuffer<HSV>& buffer) = 0;

  virtual void draw(const std::vector<HSV>& buffer) = 0;

  static HSV gammaCorrect(const HSV v, const float factor = 2.2f) {
    const float newValue = std::pow(v.value, factor);
    return HSV{v.hue, v.saturation, newValue};
  }
};
