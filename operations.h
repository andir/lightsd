//
// Created by andi on 10/11/16.
//

#ifndef LIGHTSD_OPERATIONS_H
#define LIGHTSD_OPERATIONS_H


#include <yaml-cpp/yaml.h>

#include "Operation.h"
#include "VariableStore/ConcreteValueType.h"
#include "VariableStore/VariableStore.h"
#include "VariableStore/BoundConcreteValueType.h"
#include "algorithm.h"
#include "util.h"


class ShadeOperation : public Operation {
    BoundConcreteValue<float> value;
public:
    ShadeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation("shade", store, start, end),
            value("shade/value", Operation::FLOAT_0_1, store, getValueByKey<float>("value", start, end)) {
    }

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        if (isEnabled()) {
            algorithm::MaskBuffer(value.getFloat(), buffer);
        }
    }

private:
    float parse_value(YAML::const_iterator start, YAML::const_iterator end) {
        for (; start != end; start++) {
            const auto &v = *start;
            if (v.first.as<std::string>() == "value") {
                return v.second.as<float>();
            }
        }
        return 1.0f;
    }
};

class SolidColorOperation : public Operation {
    BoundConcreteValue<float> hue;
    BoundConcreteValue<float> saturation;
    BoundConcreteValue<float> value;

public:
    SolidColorOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation("color", store, start, end),
            hue("color/hue", Operation::HSV_HUE, store, getValueByKey<float>("hue", start, end)),
            saturation("color/saturation", Operation::HSV_SATURATION, store, getValueByKey<float>("saturation", start, end)),
            value("color/value",Operation::HSV_VALUE,  store, getValueByKey<float>("value", start, end)) {}


    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        if (isEnabled()) {
            const HSV color = {hue.getValue(), saturation.getValue(), value.getValue()};
            algorithm::initSolidColor(buffer, color);
        }
    }
};


class FadeOperation : public Operation {
    BoundConcreteValue<float> min;
    BoundConcreteValue<float> max;
    BoundConcreteValue<int> from;
    BoundConcreteValue<int> to;

    std::vector<float> mask;

public:
    FadeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            Operation("fade", store, start, end),
            min("fade/min", Operation::FLOAT_0_1, store, getValueByKey<float>("min", start, end)),
            max("fade/max", Operation::FLOAT_0_1, store, getValueByKey<float>("max", start, end)),
            from("fade/from", Operation::INT, store, getValueByKey<int>("from", start, end)),
            to("fade/to", Operation::INT, store, getValueByKey<int>("to", start, end)) {

        // FIXME: as soon as the vars are observable recalculate the mask on change
        if (to.getInteger() <= from.getInteger() || min.getFloat() >= max.getFloat())
            return;

        const size_t length = (to.getInteger() - from.getInteger());
        const float diff = max.getFloat() - min.getFloat();
        const float step = diff / length;
        for (size_t i = 0; i < length; i++) {
            mask.push_back(step * i);
        }
    }

    void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        if (isEnabled()) {
            algorithm::PartialMaskBuffer(mask, buffer, from.getInteger());
        }
    }
};

#endif //LIGHTSD_OPERATIONS_H
