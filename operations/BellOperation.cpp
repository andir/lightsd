#include "BellOperation.h"

#include <chrono>

BellOperation::BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation("bell", store, begin, end),
        state(0),
        hue("bell/hue", Operation::HSV_HUE, store, getValueByKey<float>("hue", begin, end, 0.0f)),
        saturation("bell/saturation", Operation::HSV_SATURATION, store,
                   getValueByKey<float>("saturation", begin, end, 1.0f)),
        value("bell/value", Operation::HSV_VALUE, store, getValueByKey<float>("value", begin, end, 1.0f)),

        unlock_enable("bell/unlock_enable", Operation::BOOLEAN, store, getValueByKey<bool>("unlock_enable", begin, end, false)),


        duration_milliseconds("bell/duration", Operation::INT, store,
                              getValueByKey<int>("duration", begin, end, 5000)),
        fade_milliseconds("bell/fade_duration", Operation::INT, store,
                          getValueByKey<int>("fade_duration", begin, end, 750)) {
    enabled.setBool(false);
}

Operation::BufferType BellOperation::operator()(Operation::BufferType &buffer) {
    const float perc = 0.10f;
    int time_passed = 0;

    if (state == 0) {
        time_measurment.reset();
    } else {
        time_passed = time_measurment.measure();
    }

    if (time_passed > duration_milliseconds.getValue()) {
        enabled.setBool(false);
        unlock_enable.setBool(false);
        state = 0;
        return buffer;
    }

    float shade = 0.8f;

    if (time_passed < fade_milliseconds.getValue()) {
        shade *= float(time_passed) / fade_milliseconds.getValue();
    }


    const auto pixel = HSV{hue.getValue(), saturation.getValue(), value.getValue() * shade};
    const auto other_pixel = [this]() -> HSV{
        if (unlock_enable.getValue()) {
            return HSV{120.0f, 1.0f, 1.0f};
        } else {
            return HSV{0.0f, 0.0f, 0.0f};
        }
    }();

    for (size_t i = 0; i < (*buffer).size(); i++) {
        const int p = i * perc;
        const int z = (time_passed / 1000) % 2;

        if (p % 2 == z)
            (*buffer).at(i) = pixel;
        else
            (*buffer).at(i) = other_pixel;
    }

    state++;
    return buffer;
}

void BellOperation::update() {
    if (!isEnabled()) state = 0;
}
