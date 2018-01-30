#pragma once

#include <yaml-cpp/yaml.h>
#include "../hsv.h"
#include "Output.h"
#include "UDPOutput.h"

class UDPOutputWrapper : public Output {
  UDPOutput output;

 public:
  explicit UDPOutputWrapper(const YAML::Node& params);

  virtual void draw(const AbstractBaseBuffer<HSV>& buffer);

  virtual void draw(const std::vector<HSV>& buffer);
};
