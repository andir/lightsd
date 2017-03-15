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

public:
    BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end);
    virtual ~BellOperation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer);
    virtual void update();
};


#endif //LIGHTSD_BELLOPERATION_H
