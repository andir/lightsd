#pragma once

#include "Operation.h"
#include <yaml-cpp/yaml.h>

#include "../VariableStore/BoundConcreteValueType.h"
#include "../VariableStore/VariableStore.h"

class GameOfLifeOperation : public Operation {
  bool initialized;
  size_t frame_counter;

  BoundConcreteValue<float> default_hue;
  BoundConcreteValue<float> default_saturation;
  BoundConcreteValue<float> default_value;

  BoundConcreteValue<bool> v0;
  BoundConcreteValue<bool> v1;
  BoundConcreteValue<bool> v2;
  BoundConcreteValue<bool> v3;
  BoundConcreteValue<bool> v4;
  BoundConcreteValue<bool> v5;
  BoundConcreteValue<bool> v6;
  BoundConcreteValue<bool> v7;

  BoundConcreteValue<float> speed;
  BoundConcreteValue<bool> randomizeColor;

  std::vector<HSV> output;
  std::vector<HSV> delta;

 public:
  GameOfLifeOperation(const std::string& name,
                      std::shared_ptr<VariableStore> store,
                      YAML::const_iterator start,
                      YAML::const_iterator end);

  virtual ~GameOfLifeOperation() {}

  void update(const size_t width, const size_t fps);

  virtual BufferType operator()(BufferType& buffer);
};
