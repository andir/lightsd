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


template<typename T>
static T getValueByKey(const std::string key, YAML::const_iterator start, YAML::const_iterator end) {
    for (; start != end; start++) {
        const auto &v = *start;
        if (v.first.as<std::string>() == key) {
            return v.second.as<T>();
        }
    }
    return T{};
}





class ShadeOperation : public Operation {
    BoundConcreteValue<float> value;
public:
    ShadeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            value("shade", store, getValueByKey<float>("value", start, end)) {
    }

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        algorithm::MaskBuffer(value.getFloat(), buffer);
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
            hue("hue", store, getValueByKey<float>("hue", start, end)),
            saturation("saturation", store, getValueByKey<float>("saturation", start, end)),
            value("value", store, getValueByKey<float>("value", start, end)) {}


    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        const HSV color = {hue.getValue(), saturation.getValue(), value.getValue()};
        algorithm::initSolidColor(buffer, color);
    }

private:
//
//    static HSV parse_color(YAML::const_iterator start, YAML::const_iterator end) {
//        float hue = 0.0f, saturation = 0.0f, value = 0.0f;
//
//        for (; start != end; start++) {
//            const auto &v = *start;
//            if (v.first.as<std::string>() == "hue") {
//                hue = v.second.as<float>();
//            } else if (v.first.as<std::string>() == "saturation") {
//                saturation = v.second.as<float>();
//            } else {
//                value = v.second.as<float>();
//            }
//        }
//        return HSV{hue, saturation, value};
//    }

};


class FadeOperation : public Operation {
    BoundConcreteValue<float> value;
public:

    FadeOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
            value("value", store, getValueByKey<float>("value", start, end)) {
    }

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) {
        algorithm::MaskBuffer(value.getValue(), buffer);
    }

};


//class FadeOperation : public Operation {
//    BoundConcreteValueType<float> min;
//    BoundConcreteValueType<float> max;
//    BoundConcreteValueType<int> from;
//    BoundConcreteValueType<int> to;
////    const float min;
////    const float max;
////    const size_t from;
////    const size_t to;
//
//    std::vector<float> mask;
//
//public:
//    FadeOperation(VariableStore& store, YAML::const_iterator start, YAML::const_iterator end) :
//            min("min", parse_min(start, end)),
//            max("max", parse_max(start, end)),
//            from("from", parse_from(start, end)),
//            to("to", parse_to(start, end)) {
//        // FIXME: as soon as the vars are observable recalculate the mask on change
//        if (to.getInteger() <= from || min >= max)
//            return;
//
//        const size_t length = (to - from);
//        const float diff = max - min;
//        const float step = diff / length;
//        for (size_t i = 0; i < length; i++) {
//            mask.push_back(step * i);
//        }
//    }
//
//    void operator()(const AbstractBaseBuffer<HSV> &buffer) {
//        algorithm::PartialMaskBuffer(mask, buffer, from);
//    }
//
//private:
//    static float parse_min(YAML::const_iterator start, YAML::const_iterator end) {
//        for (; start != end; start++) {
//            const auto &e = *start;
//            if (e.first.as<std::string>() == "min") {
//                return e.second.as<float>();
//            }
//        }
//        return 0.0f;
//    }
//
//    static float parse_max(YAML::const_iterator start, YAML::const_iterator end) {
//        for (; start != end; start++) {
//            const auto &e = *start;
//            if (e.first.as<std::string>() == "max") {
//                return e.second.as<float>();
//            }
//        }
//        return 1.0f;
//    }
//
//    static size_t parse_to(YAML::const_iterator start, YAML::const_iterator end) {
//        for (; start != end; start++) {
//            const auto &e = *start;
//            if (e.first.as<std::string>() == "to") {
//                return e.second.as<size_t>();
//            }
//        }
//        return 0;
//    }
//
//    static size_t parse_from(YAML::const_iterator start, YAML::const_iterator end) {
//        for (; start != end; start++) {
//            const auto &e = *start;
//            if (e.first.as<std::string>() == "from") {
//                return e.second.as<size_t>();
//            }
//        }
//        return 0;
//    }
//};

#endif //LIGHTSD_OPERATIONS_H
