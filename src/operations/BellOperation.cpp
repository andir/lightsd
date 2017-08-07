#include "BellOperation.h"

#include <chrono>

BellOperation::BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
        Operation("bell", store, begin, end),
        state(State::IDLE),
        time_passed(0),
        hue("bell/hue", Operation::HSV_HUE, store, getValueByKey<float>("hue", begin, end, 0.0f)),
        saturation("bell/saturation", Operation::HSV_SATURATION, store,
                   getValueByKey<float>("saturation", begin, end, 1.0f)),
        value("bell/value", Operation::HSV_VALUE, store, getValueByKey<float>("value", begin, end, 1.0f)),
        unlock_enable("bell/unlock_enable", Operation::BOOLEAN, store,
                      getValueByKey<bool>("unlock_enable", begin, end, false)),
        duration_milliseconds("bell/duration", Operation::INT, store,
                              getValueByKey<int>("duration", begin, end, 5000)),
        fade_milliseconds("bell/fade_duration", Operation::INT, store,
                          getValueByKey<int>("fade_duration", begin, end, 750)) {
    enabled = false;
}

float BellOperation::getShade() const {
   float shade = 0.8f;
   switch (state) {
        case State::IDLE:
                return shade;
        case State::FADE_IN: 
                return shade * (float(time_passed) / fade_milliseconds);
        case State::FADE_OUT:
                return shade * (float(duration_milliseconds - time_passed) / fade_milliseconds);
        case State::RUNNING:
        default:
                return shade;
   }
}

Operation::BufferType BellOperation::operator()(Operation::BufferType &buffer) {
    if (state == State::IDLE) return buffer;
        
    const float perc = 0.10f;
    const float shade = getShade();

    const auto pixel = HSV{hue.getValue(), saturation, value * shade};
    const auto other_pixel = [this]() -> HSV {
        if (unlock_enable) {
            return HSV{120.0f, 1.0f, 1.0f};
        } else {
            return HSV{0.0f, 0.0f, 0.0f};
        }
    }();

    for (size_t i = 0; i < (*buffer).size(); i++) {
        const int p = i * perc;
        const int z = (time_passed / 1000) % 2;

        (*buffer).at(i) = (p % 2 == z) ? pixel : other_pixel;
    }

    return buffer;
}

void BellOperation::update() {
    if (!isEnabled()) { state = State::IDLE; return; }

    time_passed = time_measurement.measure();

    switch (state) {
        case State::IDLE:
                time_measurement.reset();
                time_passed = 0;
                // fallthrough
        case State::RUNNING:
                state = State::RUNNING; // handle fall through case
                if (time_passed < fade_milliseconds) {
                        state = State::FADE_IN;
                } else if (time_passed > duration_milliseconds - fade_milliseconds) {
                        state = State::FADE_OUT;
                }
                break;
        case State::FADE_IN:
                if (time_passed > fade_milliseconds) state = State::RUNNING;
                break;
        case State::FADE_OUT:
                if (time_passed >= duration_milliseconds) {
                        // stop animation
                        state = State::IDLE;
                        enabled = false;
                        unlock_enable = false;
                        return;
                }
                break;
    }
}
