//
// Created by andi on 10/11/16.
//

#ifndef VARIABLESTORE_INVALIDVARIABLETYPEEXCEPTION_H
#define VARIABLESTORE_INVALIDVARIABLETYPEEXCEPTION_H


#include <exception>
#include <string>

class InvalidVariableTypeException : public std::exception {

    const std::string s;
public:
    InvalidVariableTypeException();

    const char *what() const throw();
};

#endif //VARIABLESTORE_INVALIDVARIABLETYPEEXCEPTION_H
