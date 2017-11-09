//
// Created by andi on 10/11/16.
//

#include "ValueType.h"

std::ostream& operator<<(std::ostream& os, const ValueType::Type t) {
  switch (t) {
    case ValueType::Type::FLOAT:
      os << "FLOAT";
      break;
    case ValueType::Type::INTEGER:
      os << "INTEGER";
      break;
    case ValueType::Type::BOOLEAN:
      os << "BOOL";
      break;
    case ValueType::Type::STRING:
      os << "STRING";
      break;
    default:
      os << "UNKNOWN";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const ValueType& obj) {
  const auto t = obj.getType();
  switch (t) {
    case ValueType::Type::FLOAT:
      os << obj.getFloat();
      break;
    case ValueType::Type::INTEGER:
      os << obj.getInteger();
      break;
    case ValueType::Type::BOOLEAN:
      os << std::boolalpha << obj.getBool();
      break;
    case ValueType::Type::STRING:
      os << obj.getString();
    default:
      os << "(unknown type)";
  }
  return os;
}

void ValueType::addOnChangeCallback(std::function<void(ValueType*)> cb) {
  if (cb != nullptr)
    callbacks.push_back(cb);
}

void ValueType::triggerCallbacks() {
  for (auto& cb : callbacks) {
    cb(this);
  }
}