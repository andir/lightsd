#pragma once

#include <yaml-cpp/yaml.h>
#include "../VariableStore/BoundConcreteValueType.h"
#include "../VariableStore/ConcreteValueType.h"
#include "../VariableStore/VariableStore.h"
#include "../algorithm.h"
#include "../hsv.h"

class SolidColorOperation : public Operation {
  BoundConcreteValue<float> hue;
  BoundConcreteValue<float> saturation;
  BoundConcreteValue<float> value;

 public:
  SolidColorOperation(const std::string& name,
                      std::shared_ptr<VariableStore> store,
                      YAML::const_iterator start,
                      YAML::const_iterator end)
      : Operation(name, store, start, end),
        hue(name + "/hue",
            Operation::HSV_HUE,
            store,
            getValueByKey<float>("hue", start, end)),
        saturation(name + "/saturation",
                   Operation::HSV_SATURATION,
                   store,
                   getValueByKey<float>("saturation", start, end)),
        value(name + "/value",
              Operation::HSV_VALUE,
              store,
              getValueByKey<float>("value", start, end)) {}

  virtual BufferType operator()(BufferType& buffer) {
    const HSV color = {hue, saturation, value};
    algorithm::initSolidColor(*buffer, color);
    return buffer;
  }
};
