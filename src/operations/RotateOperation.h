#pragma once

#include <yaml-cpp/yaml.h>

#include "Operation.h"
#include "../VariableStore/BoundConcreteValueType.h"
#include "../VariableStore/VariableStore.h"
#include "../hsv.h"
#include "../utils/util.h"


class RotateOperation : public Operation {
    float step;
    BoundConcreteValue<float> step_width;
    MeasureTime<> timeMeasurement;
public:
    RotateOperation(const std::string& name, VariableStore &store, YAML::const_iterator start, YAML::const_iterator end);

    virtual ~RotateOperation();

    virtual Operation::BufferType operator()(Operation::BufferType &buffer);
};
