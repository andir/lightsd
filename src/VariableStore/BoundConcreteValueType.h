#pragma once

#include <memory>
#include "ConcreteValueType.h"
#include "VariableStore.h"

template<typename EnclosedValue>
class BoundConcreteValue : public ValueType {
    using CallbackType = typename ConcreteValueType<EnclosedValue>::CallbackType;
    std::string name;
    std::weak_ptr<VariableStore> store;
    std::shared_ptr<ConcreteValueType<EnclosedValue >> value;

public:
    BoundConcreteValue(const std::string& name, const std::string& description, std::shared_ptr<VariableStore> &store,
                       EnclosedValue initial_value, CallbackType cb = nullptr) :
            name(name),
            store(store),
            value(std::make_shared<ConcreteValueType<EnclosedValue >>(initial_value, cb)) {
        store->registerVar(name, description, value);
    }

    ~BoundConcreteValue() {
        std::shared_ptr<VariableStore> storep = store.lock();
        if (storep != nullptr) {
            storep->unregisterVar(name);
        }
    }

    virtual Type getType() const {
        return value->getType();
    }

    virtual float getFloat() const {
        if (std::is_same<float, EnclosedValue>::value) {
            return value->getFloat();
        } else {
            assert(false && "Invalid getter used");
        }
    }

    virtual int getInteger() const {
        if (std::is_same<int, EnclosedValue>::value) {
            return value->getInteger();
        } else {
            assert(false && "Invalid getter used");
        }
    }

    virtual bool getBool() const {
        if (std::is_same<bool, EnclosedValue>::value) {
            return value->getBool();
        } else {
            assert(false && "Invalid getter used");
        }
    }

    virtual void setFloat(const float v) {
        if (std::is_same<float, EnclosedValue>::value) {
            value->setFloat(v);
        } else {
            // FIXME: add some fast way of error reporting
            assert(false && "Invalid setter used");
        }
    }

    virtual void setInteger(const int v) {
        if (std::is_same<int, EnclosedValue>::value) {
            value->setInteger(v);
        } else {
            // FIXME: add some fast way of error reporting
            assert(false && "Invalid setter used");
        }
    }

    virtual void setBool(const bool v) {
        if (std::is_same<bool, EnclosedValue>::value) {
            value->setBool(v);
        } else {
            // FIXME: add some fast way of error reporting
            assert(false && "Invalid setter used");
        }
    }

    EnclosedValue getValue() const {
        return value->getValue();
    }

    operator EnclosedValue() const {
        return getValue();
    }

    BoundConcreteValue& operator=(const EnclosedValue v) {
        if (std::is_same<bool, EnclosedValue>::value) {
            value->setBool(v);
        } else if (std::is_same<int, EnclosedValue>::value) {
            value->setInteger(v);
        } else if (std::is_same<float, EnclosedValue>::value) {
            value->setFloat(v);
        }

        return *this;
    }
};
