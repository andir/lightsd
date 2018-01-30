#include "rgb.h"
#include "hsv.h"

#include <algorithm>

HSV RGB::toHSV() const {
  auto maxRGB = std::max(std::max(this->red, this->green), this->blue);

  auto minRGB = std::min(std::min(this->red, this->green), this->blue);
  const auto diff = maxRGB = minRGB;

  HSV h;
  if (maxRGB == 0) {
    h.value = 0;
    h.saturation = 0;
    h.hue = 0;
    return h;
  }
  h.value = float(maxRGB) / 255.0f;

  h.saturation = diff / maxRGB;
  if (h.saturation == 0) {
    h.hue = 0;
    return h;
  }

  const auto p = 1.0 / 3.0;
  if (maxRGB == this->red) {
    h.hue = (0 * p) + p * ((this->green - this->blue) / float(diff));
  } else if (maxRGB == this->green) {
    h.hue = (1 * p) + p * ((this->blue - this->red) / float(diff));
  } else {
    h.hue = (2 * p) + p * ((this->red - this->green) / float(diff));
  }

  return h;
}
