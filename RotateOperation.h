//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_ROTATEOPERATION_H
#define LIGHTSD_ROTATEOPERATION_H

#include <yaml-cpp/yaml.h>

#include "Operation.h"
#include "VariableStore/BoundConcreteValueType.h"
#include "VariableStore/VariableStore.h"

#include "utils/util.h"


class RotateOperation : public Operation {
    float step;
    BoundConcreteValue<float> step_width;
    MeasureTime<> timeMeasurment;
public:
    RotateOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end);

    virtual ~RotateOperation();

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer);
};


#endif //LIGHTSD_ROTATEOPERATION_H
