#pragma once

#include "Operation.h"

class BellOperation : public Operation {
    enum State {
        IDLE,
        RUNNING,
        FADE_IN,
        FADE_OUT,
    } state;

    inline const char* toString(State s) {
        switch (s) {
                case State::IDLE: return "IDLE";
                case State::RUNNING: return "RUNNING";
                case State::FADE_IN: return "FADE_IN";
                case State::FADE_OUT: return "FADE_OUT";
                default: return "UNKNOWN";
        }
    }

    int64_t time_passed;

    MeasureTime<> time_measurement;

    BoundConcreteValue<float> hue;
    BoundConcreteValue<float> saturation;
    BoundConcreteValue<float> value;

    BoundConcreteValue<bool> unlock_enable;
    BoundConcreteValue<bool> transparent;

    BoundConcreteValue<int> duration_milliseconds;
    BoundConcreteValue<int> fade_milliseconds;

    float getShade() const;

public:
    BellOperation(const std::string& name, std::shared_ptr<VariableStore> store, YAML::const_iterator begin, YAML::const_iterator end);

    virtual ~BellOperation() {}

    virtual Operation::BufferType operator()(Operation::BufferType &buffer);

    virtual void update(const size_t width, const size_t fps);
};
