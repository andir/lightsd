#pragma once

#include <exception>
#include <string>

class InvalidVariableTypeException : public std::exception {
  const std::string s;

 public:
  InvalidVariableTypeException();

  const char* what() const throw();
};
