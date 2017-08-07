#pragma once

#include "Operation.h"

class BellOperation : public Operation {
    int state;

    MeasureTime<> time_measurement;

    BoundConcreteValue<float> hue;
    BoundConcreteValue<float> saturation;
    BoundConcreteValue<float> value;

    BoundConcreteValue<bool> unlock_enable;

    BoundConcreteValue<int> duration_milliseconds;
    BoundConcreteValue<int> fade_milliseconds;

public:
    BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end);

    virtual ~BellOperation() {}

    virtual Operation::BufferType operator()(Operation::BufferType &buffer);

    virtual void update();
};
