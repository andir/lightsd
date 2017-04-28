#ifndef VARIABLESTORE_VALUETYPE_H
#define VARIABLESTORE_VALUETYPE_H

#include <ostream>

class ValueType {
public:
    enum class Type {
        FLOAT, INTEGER, UNKNOWN, BOOLEAN
    };

    virtual Type getType() const = 0;

    virtual bool getBool() const = 0;

    virtual void setBool(const bool) = 0;

    virtual float getFloat() const = 0;

    virtual int getInteger() const = 0;

    virtual void setFloat(const float) = 0;

    virtual void setInteger(const int) = 0;

};


std::ostream &operator<<(std::ostream &os, const ValueType &obj);
std::ostream &operator<<(std::ostream &os, const ValueType::Type t);


#endif //VARIABLESTORE_VALUETYPE_H
