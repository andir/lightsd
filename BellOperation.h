//
// Created by andi on 11/20/16.
//

#ifndef LIGHTSD_BELLOPERATION_H
#define LIGHTSD_BELLOPERATION_H

#include "Operation.h"

class BellOperation : public Operation {
    int state;
    int milliseconds;

public:
    BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end);
    virtual ~BellOperation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer);
};


#endif //LIGHTSD_BELLOPERATION_H
