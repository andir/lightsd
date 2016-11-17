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
    std::string name;
    VariableStore &store;
    std::shared_ptr <ConcreteValueType<EnclosedValue >> value;

public:
    BoundConcreteValue(const std::string name, const std::string description, VariableStore &store, EnclosedValue initial_value) :
            name(name),
            store(store),
            value(std::make_shared<ConcreteValueType<EnclosedValue >>(initial_value)) {
        store.registerVar(name, description, value);
    }

    ~BoundConcreteValue() {
        store.unregisterVar(name);
    }

    virtual Type getType() const {
        return value->getType();
    }

    virtual float getFloat() const {
        return value->getFloat();
    }

    virtual int getInteger() const {
        return value->getInteger();
    }

    virtual void setFloat(const float v) {
        value->setFloat(v);
    }

    virtual void setInteger(const int v) {
        value->setInteger(v);
    }

    EnclosedValue getValue() const {
        return value->getValue();
    }
};


#endif //LIGHTSD_BOUNDCONCRETEVALUETYPE_H
