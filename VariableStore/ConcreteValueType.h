//
// Created by andi on 10/11/16.
//

#ifndef VARIABLESTORE_CONCRETEVALUETYPE_H
#define VARIABLESTORE_CONCRETEVALUETYPE_H

#include "ValueType.h"
#include "InvalidVariableTypeException.h"

namespace {
    template<typename T>
    static constexpr ValueType::Type determineType() {
        static_assert("Not implemented");
        return ValueType::Type::UNKNOWN;
    }

    template<>
    constexpr ValueType::Type determineType<float>() {
        return ValueType::Type::FLOAT;
    }

    template<>
    constexpr ValueType::Type determineType<int>() {
        return ValueType::Type::INTEGER;
    }
};

template<typename InternalValueType>
class ConcreteValueType : public ValueType {
public:
    using CallbackType = std::function<void (const InternalValueType&)>;

private:
    InternalValueType value;
    Type type;
    CallbackType callback;

    inline void notify(const InternalValueType t) {
        if (callback != nullptr) {
            callback(getValue());
        }
    }

public:
    ConcreteValueType(InternalValueType initial_value, CallbackType cb = nullptr) : value(initial_value), callback(cb) {

    }

    inline Type getType() const {
        return determineType<InternalValueType>();
    }

    virtual float getFloat() const {
        if (std::is_same<float, InternalValueType>::value) {
                return getValue<float>();
        } else {
                assert(false && "Invalid getter used");
        }
    }

    virtual int getInteger() const {
        if (std::is_same<int, InternalValueType>::value) {
                return getValue<int>();
        } else {
                assert(false && "Invalid getter used");
        }
    }

    virtual void setInteger(const int v) {
        if (std::is_same<int, InternalValueType>::value) {
                notify(v);
                setValue<InternalValueType>(v);
        } else {
                assert(false && "Invalid setter used");
        }
    }

    virtual void setFloat(const float v) {
        if (std::is_same<float, InternalValueType>::value) {
                notify(v);
                setValue<InternalValueType>(v);
        } else {
                assert(false && "Invalid setter used");
        }
    }

    virtual inline InternalValueType getValue() const {
        return getValue<InternalValueType>();
    }

    virtual inline void setValue(InternalValueType v) {
        value = v;
    }


private:
    template<typename TargetType>
    TargetType
    getValue(typename std::enable_if<!std::is_same<InternalValueType, TargetType>::value, int>::type = 0) const {
        throw InvalidVariableTypeException();
    }

    template<typename TargetType>
    TargetType
    getValue(typename std::enable_if<std::is_same<InternalValueType, TargetType>::value, int>::type = 0) const {
        return value;
    }


    template<typename TargetType>
    void setValue(typename std::enable_if<!std::is_same<InternalValueType, TargetType>::value, TargetType>::type v) {
        throw InvalidVariableTypeException();
    }

    template<typename TargetType>
    void setValue(typename std::enable_if<std::is_same<InternalValueType, TargetType>::value, TargetType>::type v) {
        value = v;
    }
};


#endif //VARIABLESTORE_CONCRETEVALUETYPE_H
