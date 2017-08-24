#pragma once

#include <atomic>

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

    template<>
    constexpr ValueType::Type determineType<bool>() {
        return ValueType::Type::BOOLEAN;
    }
};

template<typename InternalValueType>
class ConcreteValueType : public ValueType {
private:
    std::atomic<InternalValueType> value;
    Type type;

    inline void notify() {
        std::cerr << "triggering callbacks " << std::endl;
        triggerCallbacks();
    }

public:
    ConcreteValueType(InternalValueType initial_value, CallbackType cb = nullptr) : value(initial_value) {
        addOnChangeCallback(cb);
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

    virtual bool getBool() const {
        if (std::is_same<bool, InternalValueType>::value) {
            return getValue<bool>();
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
            setValue<InternalValueType>(v);
            notify();
        } else {
            assert(false && "Invalid setter used");
        }
    }

    virtual void setFloat(const float v) {
        if (std::is_same<float, InternalValueType>::value) {
            setValue<InternalValueType>(v);
            notify();
        } else {
            assert(false && "Invalid setter used");
        }
    }

    virtual void setBool(const bool v) {
        if (std::is_same<bool, InternalValueType>::value) {
            setValue<InternalValueType>(v);
            notify();
        } else {
            assert(false && "Invalid setter used");
        }
    }

    virtual inline InternalValueType getValue() const {
        return getValue < InternalValueType > ();
    }

    virtual inline void setValue(InternalValueType v) {
        value = v;
    }


private:
    template<typename TargetType>
    TargetType
    getValue(typename std::enable_if<!std::is_same<InternalValueType, TargetType>::value, int>::type = 0) const {
        static_assert((!std::is_same<InternalValueType, TargetType>::value) && "You did something wrong.");
        std::cerr << "tried to get variable. Wrong getter for this type: " << determineType<TargetType>() << std::endl;
        std::cerr << "correct type for this var is: " << determineType<InternalValueType>() << std::endl;
        throw InvalidVariableTypeException();
    }

    template<typename TargetType>
    TargetType
    getValue(typename std::enable_if<std::is_same<InternalValueType, TargetType>::value, int>::type = 0) const {
        return value;
    }


    template<typename TargetType>
    void setValue(typename std::enable_if<!std::is_same<InternalValueType, TargetType>::value, TargetType>::type v) {
        static_assert("You did something wrong.");
        std::cerr << "tried to set variable" << std::endl;
        throw InvalidVariableTypeException();
    }

    template<typename TargetType>
    void setValue(typename std::enable_if<std::is_same<InternalValueType, TargetType>::value, TargetType>::type v) {
        value = v;
    }


};
