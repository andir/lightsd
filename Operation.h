//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_OPERATION_H
#define LIGHTSD_OPERATION_H

#include "buffer.h"
#include "hsv.h"

class Operation {
public:
    Operation() {}

    virtual ~Operation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) = 0;
};

#endif //LIGHTSD_OPERATION_H
