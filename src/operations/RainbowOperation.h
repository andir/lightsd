#pragma once

#include "../VariableStore/VariableStore.h"
#include "Operation.h"
#include <yaml-cpp/yaml.h>

class RainbowOperation : public Operation {
 public:
  RainbowOperation(const std::string& name,
                   std::shared_ptr<VariableStore> store,
                   YAML::const_iterator begin,
                   YAML::const_iterator end);

  virtual ~RainbowOperation() {}

  virtual Operation::BufferType operator()(BufferType& buffer);
};
