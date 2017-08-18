#include "RotateOperation.h"
#include <yaml-cpp/yaml.h>

RotateOperation::RotateOperation(const std::string& name, std::shared_ptr<VariableStore> store, YAML::const_iterator start, YAML::const_iterator end) :
        Operation(name, store, start, end),
        step(0.0),
        step_width(
                name + "/step_width",
                Operation::FLOAT_0_1,
                store,
                getValueByKey<float>(
                        "step_width",
                        start,
                        end)) {
}

RotateOperation::~RotateOperation() {}

Operation::BufferType RotateOperation::operator()(Operation::BufferType &buffer) {
    // time since last rotation in milliseconds
    uint64_t time_elapsed = timeMeasurement.measure();
    timeMeasurement.reset();

    const double steps_per_ms = step_width / 1000.0;

    step += steps_per_ms * time_elapsed;
    const int64_t integer_part = uint64_t(step);
    step = (integer_part % buffer->size()) + (step - integer_part);

    const size_t offset = step;
    auto end = &(*buffer).at(buffer->size() - 1);

    //std::cerr << "steps_per_ms:" << steps_per_ms << " step: " << step << " offset: " << offset << " end: " << end << std::endl;
    std::rotate(&(*buffer).at(0), &(*buffer).at(0) + offset, end + 1);

    step = std::fmod(step, float(buffer->size()));
    return buffer;
}
