#pragma once

#include <yaml-cpp/yaml.h>
#include "Operation.h"
#include "../VariableStore/VariableStore.h"

class RainbowOperation : public Operation {
public:
    RainbowOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end);

    virtual ~RainbowOperation() {}

    virtual Operation::BufferType operator()(BufferType &buffer);
};
