//
// Created by andi on 10/11/16.
//

#include "ValueType.h"

std::ostream &operator<<(std::ostream &os, const ValueType &obj) {
    if (obj.getType() == ValueType::Type::FLOAT) {
        os << obj.getFloat();
    } else {
        os << obj.getInteger();
    }
    return os;
}