//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_OPERATION_H
#define LIGHTSD_OPERATION_H

#include "../Buffer.h"
#include "../VariableStore/BoundConcreteValueType.h"
#include "../hsv.h"
#include "../utils/util.h"

class Operation {
protected:
    const std::string name;
    BoundConcreteValue<int> enabled;

    static inline std::string concat(const std::string a, const std::string b) {
        return a + b;
    }

protected:


    inline bool isEnabled() const {
        return enabled.getInteger() >= 1;
    }

public:
    Operation(const std::string name, VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
            name(name),
            enabled(concat(name, "/enabled"), Operation::BOOLEAN, store, getValueByKey<int>("enabled", begin, end, 1)) {}

    virtual ~Operation() {}

    virtual void operator()(const AbstractBaseBuffer<HSV> &buffer) = 0;

    inline std::string getName() const { return name; }

    static const std::string HSV_HUE;
    static const std::string HSV_SATURATION;
    static const std::string HSV_VALUE;

    static const std::string BOOLEAN;

    static const std::string FLOAT_0_1;

    static const std::string INT;

    static const std::string FLOAT;
};

#endif //LIGHTSD_OPERATION_H
