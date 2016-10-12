//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_RAINBOWOPERATION_H
#define LIGHTSD_RAINBOWOPERATION_H

#include <yaml-cpp/yaml.h>
#include "Operation.h"
#include "VariableStore/VariableStore.h"

class RainbowOperation : public Operation {
public:
    RainbowOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end);

    virtual ~RainbowOperation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer);
};



#endif //LIGHTSD_RAINBOWOPERATION_H
