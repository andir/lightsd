#pragma once

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


    static inline std::string concat(const std::string& a, const std::string& b) {
        return a + b;
    }

public:
    using ContainerType = AbstractBaseBuffer<HSV>;
    using BufferType = std::shared_ptr<ContainerType >;

    Operation(const std::string& name, std::shared_ptr<VariableStore> store, YAML::const_iterator begin, YAML::const_iterator end) :
            name(name),
            enabled(concat(name, "/enabled"), Operation::BOOLEAN, store, getValueByKey<bool>("enabled", begin, end, 1)),
            alpha(concat(name, "/alpha"), Operation::FLOAT, store, getValueByKey<float>("alpha", begin, end, 1)) {}

    virtual ~Operation() {}

    virtual BufferType operator()(BufferType &buffer) = 0;

    inline std::string getName() const { return name; }

    static const MqttVarType HSV_HUE;
    static const MqttVarType HSV_SATURATION;
    static const MqttVarType HSV_VALUE;

    static const MqttVarType BOOLEAN;

    static const MqttVarType FLOAT_0_1;

    static const MqttVarType INT;

    static const MqttVarType FLOAT;

    static const MqttVarType STRING;

    inline bool isEnabled() const {
        return enabled;
    }

    virtual void update(const size_t width, const size_t fps) {};

    virtual float getAlpha() const { return alpha; }
};
