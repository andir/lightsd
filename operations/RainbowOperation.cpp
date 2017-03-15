//
// Created by andi on 10/12/16.
//

#include "RainbowOperation.h"
#include "../algorithm.h"
#include <yaml-cpp/yaml.h>


RainbowOperation::RainbowOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation("rainbow", store, begin, end) {
}

void RainbowOperation::operator()(const AbstractBaseBuffer<HSV> &buffer) {
    algorithm::initRainbow(buffer);
}
