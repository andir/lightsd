//
// Created by andi on 10/12/16.
//

#include "RotateOperation.h"
#include "operations.h"
#include <yaml-cpp/yaml.h>

RotateOperation::RotateOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
        Operation("rotate", store, start, end),
        step(0),
        step_width(
                "rotate/step_width",
                store,
                getValueByKey<int>(
                        "step_width",
                        start,
                        end)) {
}

RotateOperation::~RotateOperation() {}

void RotateOperation::operator()(const AbstractBaseBuffer <HSV> &buffer) {

    const size_t current_position = (step % buffer.count()) * step_width.getInteger();
    const size_t offset = (current_position + step_width.getInteger()) % buffer.count();

    std::rotate(&buffer.at(0), &buffer.at(0) + offset, &*buffer.end());

    step++;
}