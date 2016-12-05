//
// Created by andi on 10/12/16.
//

#include "RotateOperation.h"
#include "operations.h"
#include <yaml-cpp/yaml.h>

RotateOperation::RotateOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
        Operation("rotate",   store, start, end),
        step(0),
        step_width(
                "rotate/step_width",
                Operation::INT,
                store,
                getValueByKey<int>(
                        "step_width",
                        start,
                        end)) {
}

RotateOperation::~RotateOperation() {}

void RotateOperation::operator()(const AbstractBaseBuffer<HSV> &buffer) {

    const size_t current_position = (step % buffer.count()) * step_width.getInteger();
    const size_t offset = (current_position + step_width.getInteger()) % buffer.count();

    auto end = &buffer.at(buffer.count() - 1);

    std::rotate(&buffer.at(0), &buffer.at(0) + offset, end);

    step++;
}