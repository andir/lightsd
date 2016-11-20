#include "BellOperation.h"

#include <chrono>

BellOperation::BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
    Operation("bell", store, begin, end),
    state(0),
    milliseconds(2000)
{
    enabled.setInteger(0);
}

void BellOperation::operator()(const AbstractBaseBuffer<HSV> &buffer) {
    const float perc = 0.10f;
    if (isEnabled()) {

        unsigned int time_passed;

        if (state == 0) {
            time_measurment.reset();
        } else {
            time_passed = time_measurment.measure();
        }


        float shade = 0.0f;

        if (time_passed > milliseconds) {
            shade = 1.0f;
        } else {
            shade = milliseconds / time_passed;
        }
        int t = ((state / 10) % 6 == 0 ? 1 : 2);
        for (size_t i = 0; i < buffer.size(); i++) {
            if (int(i / 10) % 2 == t)
                buffer.at(i) = HSV{0.0f,1.0f, 1.0f * shade};
            else
                buffer.at(i) = HSV{0.0f,0.0f,0.0f};
        }
        state ++;
    } else {
        state = 0;
    }
}