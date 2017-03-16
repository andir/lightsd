#include "BellOperation.h"

#include <chrono>

BellOperation::BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation("bell", store, begin, end),
        state(0),
        milliseconds(2000) {
    enabled.setInteger(0);
}

void BellOperation::operator()(const AbstractBaseBuffer<HSV> &buffer) {
    const float perc = 0.10f;
    unsigned int time_passed = 0;

    if (state == 0) {
        time_measurment.reset();
    } else {
        time_passed = time_measurment.measure();
    }

    float shade = 0.8f;

    if (time_passed < milliseconds) {
        shade *= float(time_passed) / milliseconds;
    }

    for (size_t i = 0; i < buffer.size(); i++) {
        const int p = i * perc;

        int z = (time_passed / 1000) % 2;

        if (p % 2 == z)
            buffer.at(i) = HSV{0.0f, 1.0f, 1.0f * shade};
        else
            buffer.at(i) = HSV{0.0f, 0.0f, 0.0f};
    }
    state++;
}

void BellOperation::update() {
    if (!isEnabled()) state =0;
}