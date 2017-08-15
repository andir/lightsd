//
// Created by andi on 10/12/16.
//

#include "RainbowOperation.h"
#include "../algorithm.h"
#include <yaml-cpp/yaml.h>


RainbowOperation::RainbowOperation(const std::string& name, VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation(name, store, begin, end) {
}

Operation::BufferType RainbowOperation::operator()(Operation::BufferType &buffer) {
    algorithm::initRainbow(*buffer);
    return buffer;
}
