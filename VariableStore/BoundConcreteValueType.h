//
// Created by andi on 10/11/16.
//

#ifndef LIGHTSD_BOUNDCONCRETEVALUETYPE_H
#define LIGHTSD_BOUNDCONCRETEVALUETYPE_H

#include <memory>
#include "ConcreteValueType.h"
#include "VariableStore.h"

template<typename EnclosedValue>
class BoundConcreteValue : public ValueType {
    using CallbackType = std::function<void (const EnclosedValue&)>;
    std::string name;
    VariableStore &store;
    std::shared_ptr <ConcreteValueType<EnclosedValue >> value;
    CallbackType callback;

    inline void notify(const EnclosedValue t) {
        if (callback != nullptr) {
            callback(value->getValue());
        }
    }

public:
    BoundConcreteValue(const std::string name, const std::string description, VariableStore &store, EnclosedValue initial_value, CallbackType cb = nullptr) :
            name(name),
            store(store),
            value(std::make_shared<ConcreteValueType<EnclosedValue >>(initial_value)) {
        store.registerVar(name, description, value);
        callback = cb;
    }

    ~BoundConcreteValue() {
        store.unregisterVar(name);
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

    virtual void setFloat(const float v) {
        if (std::is_same<float, EnclosedValue>::value) {
            value->setFloat(v);
            notify(v);
        } else {
            // FIXME: add some fast way of error reporting
            assert(false && "Invalid setter used");
        }
    }

    virtual void setInteger(const int v) {
        if (std::is_same<int, EnclosedValue>::value) {
            value->setInteger(v);
            notify(v);
        } else {
            // FIXME: add some fast way of error reporting
            assert(false && "Invalid setter used");
        }
    }

    EnclosedValue getValue() const {
        return value->getValue();
    }
};


#endif //LIGHTSD_BOUNDCONCRETEVALUETYPE_H
