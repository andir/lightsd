#include "BellOperation.h"

#include <chrono>

BellOperation::BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation("bell", store, begin, end),
        state(0),
        milliseconds(2000),
        hue("bell/hue", Operation::HSV_HUE, store, getValueByKey<float>("hue", begin, end, 0.0f)),
        saturation("bell/saturation", Operation::HSV_SATURATION, store, getValueByKey<float>("saturation", begin, end, 1.0f)),
        value("bell/value", Operation::HSV_VALUE, store, getValueByKey<float>("value", begin, end, 1.0f)) {
    enabled.setBool(false);
}

Operation::BufferType BellOperation::operator()(Operation::BufferType &buffer) {
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


    const auto pixel = HSV{hue.getValue(), saturation.getValue(), value.getValue() * shade};

    for (size_t i = 0; i < (*buffer).size(); i++) {
        const int p = i * perc;

        const int z = (time_passed / 1000) % 2;

        if (p % 2 == z)
            (*buffer).at(i) = pixel;
        else
            (*buffer).at(i) = HSV{0.0f, 0.0f, 0.0f};
    }

    state++;
    return buffer;
}

void BellOperation::update() {
    if (!isEnabled()) state = 0;
}
