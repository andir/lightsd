//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_OPERATION_H
#define LIGHTSD_OPERATION_H

#include "../Buffer.h"
#include "../VariableStore/BoundConcreteValueType.h"
#include "../hsv.h"
#include "../utils/util.h"

#include <memory>

class Config;

class Operation {
protected:
    const std::string name;
    BoundConcreteValue<bool> enabled;
    BoundConcreteValue<float> alpha;


    static inline std::string concat(const std::string a, const std::string b) {
        return a + b;
    }

public:

    using BufferType = std::shared_ptr<AbstractBaseBuffer<HSV> >;

    Operation(const std::string name, VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
            name(name),
            enabled(concat(name, "/enabled"), Operation::BOOLEAN, store, getValueByKey<bool>("enabled", begin, end, 1)),
            alpha(concat(name, "/alpha"), Operation::FLOAT, store, getValueByKey<float>("alpha", begin, end, 1)) {}

    virtual ~Operation() {}

    virtual BufferType operator()(BufferType &buffer) = 0;

    inline std::string getName() const { return name; }

    static const std::string HSV_HUE;
    static const std::string HSV_SATURATION;
    static const std::string HSV_VALUE;

    static const std::string BOOLEAN;

    static const std::string FLOAT_0_1;

    static const std::string INT;

    static const std::string FLOAT;

    inline bool isEnabled() const {
        return enabled.getBool();
    }
    virtual void update(const Config * const ) {};
    virtual float getAlpha() const { return alpha.getValue(); }
};

#endif //LIGHTSD_OPERATION_H
