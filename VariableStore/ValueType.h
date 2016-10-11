#ifndef VARIABLESTORE_VALUETYPE_H
#define VARIABLESTORE_VALUETYPE_H

#include <ostream>

class ValueType {
public:
    enum class Type { FLOAT, INTEGER, UNKNOWN };

    virtual Type getType() const = 0;
    virtual float getFloat() const = 0;
    virtual int getInteger() const = 0;

    virtual void setFloat(const float) = 0;
    virtual void setInteger(const int) = 0;
};


std::ostream &operator<<(std::ostream &os, const ValueType& obj);


#endif //VARIABLESTORE_VALUETYPE_H
