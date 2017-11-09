//
// Created by andi on 10/11/16.
//

#include "InvalidVariableTypeException.h"

InvalidVariableTypeException::InvalidVariableTypeException()
    : s("Invalid variable type requested.") {}

const char* InvalidVariableTypeException::what() const throw() {
  return s.c_str();
}