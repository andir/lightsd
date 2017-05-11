//
// Created by andi on 11/20/16.
//

#ifndef LIGHTSD_BELLOPERATION_H
#define LIGHTSD_BELLOPERATION_H

#include "Operation.h"

class BellOperation : public Operation {
    int state;
    unsigned int milliseconds;

    MeasureTime<> time_measurment;

    BoundConcreteValue<float> hue;
    BoundConcreteValue<float> saturation;
    BoundConcreteValue<float> value;

public:
    BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end);

    virtual ~BellOperation() {}

    virtual Operation::BufferType operator()(Operation::BufferType &buffer);

    virtual void update();
};


#endif //LIGHTSD_BELLOPERATION_H
