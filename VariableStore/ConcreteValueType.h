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

template<typename IntervalValueType>
class ConcreteValueType : public ValueType {

    IntervalValueType value;
    Type type;

public:
    ConcreteValueType(IntervalValueType initial_value) : value(initial_value) {

    }

    inline Type getType() const {
        return determineType<IntervalValueType>();
    }

    virtual float getFloat() const {
        return getValue<float>();
    }

    virtual int getInteger() const {
        return getValue<int>();
    }

    virtual void setInteger(const int v) {
        setValue<IntervalValueType>(v);
    }

    virtual void setFloat(const float v) {
        setValue<IntervalValueType>(v);
    }

    virtual inline IntervalValueType getValue() const {
        return getValue<IntervalValueType>();
    }

    virtual inline void setValue(IntervalValueType v) {
        value = v;
    }


private:
    template<typename TargetType>
    TargetType
    getValue(typename std::enable_if<!std::is_same<IntervalValueType, TargetType>::value, int>::type = 0) const {
        throw InvalidVariableTypeException();
    }

    template<typename TargetType>
    TargetType
    getValue(typename std::enable_if<std::is_same<IntervalValueType, TargetType>::value, int>::type = 0) const {
        return value;
    }


    template<typename TargetType>
    void setValue(typename std::enable_if<!std::is_same<IntervalValueType, TargetType>::value, TargetType>::type v) {
        throw InvalidVariableTypeException();
    }

    template<typename TargetType>
    void setValue(typename std::enable_if<std::is_same<IntervalValueType, TargetType>::value, TargetType>::type v) {
        value = v;
    }
};


#endif //VARIABLESTORE_CONCRETEVALUETYPE_H
