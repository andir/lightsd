#pragma once

#include <yaml-cpp/yaml.h>
#include "../VariableStore/ConcreteValueType.h"
#include "../VariableStore/VariableStore.h"
#include "../VariableStore/BoundConcreteValueType.h"
#include "../algorithm.h"
#include "../hsv.h"


class SolidColorOperation : public Operation {
    BoundConcreteValue<float> hue;
    BoundConcreteValue<float> saturation;
    BoundConcreteValue<float> value;

public:
    SolidColorOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation("color", store, start, end),
            hue("color/hue", Operation::HSV_HUE, store, getValueByKey<float>("hue", start, end)),
            saturation("color/saturation", Operation::HSV_SATURATION, store,
                       getValueByKey<float>("saturation", start, end)),
            value("color/value", Operation::HSV_VALUE, store, getValueByKey<float>("value", start, end)) {}


    virtual BufferType operator()(BufferType &buffer) {
        const HSV color = {hue.getValue(), saturation.getValue(), value.getValue()};
        algorithm::initSolidColor(*buffer, color);
        return buffer;
    }
};


