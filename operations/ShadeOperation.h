#pragma once

#include <yaml-cpp/yaml.h>
#include "../VariableStore/ConcreteValueType.h"
#include "../VariableStore/VariableStore.h"
#include "../VariableStore/BoundConcreteValueType.h"
#include "../algorithm.h"
#include "../hsv.h"

class ShadeOperation : public Operation {
    BoundConcreteValue<float> value;
public:
    ShadeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation("shade", store, start, end),
            value("shade/value", Operation::FLOAT_0_1, store, getValueByKey<float>("value", start, end)) {
    }

    virtual BufferType operator()(BufferType &buffer) {
        algorithm::MaskBuffer(value, *buffer);
        return buffer;
    }
};
