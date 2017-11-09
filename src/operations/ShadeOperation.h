#pragma once

#include "../VariableStore/BoundConcreteValueType.h"
#include "../VariableStore/ConcreteValueType.h"
#include "../VariableStore/VariableStore.h"
#include "../algorithm.h"
#include "../hsv.h"
#include <yaml-cpp/yaml.h>

class ShadeOperation : public Operation {
  BoundConcreteValue<float> value;

 public:
  ShadeOperation(const std::string& name,
                 std::shared_ptr<VariableStore> store,
                 YAML::const_iterator start,
                 YAML::const_iterator end)
      : Operation(name, store, start, end),
        value(name + "/value",
              Operation::FLOAT_0_1,
              store,
              getValueByKey<float>("value", start, end)) {}

  virtual BufferType operator()(BufferType& buffer) {
    algorithm::MaskBuffer(value, *buffer);
    return buffer;
  }
};
