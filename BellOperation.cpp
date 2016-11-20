#include "BellOperation.h"



BellOperation::BellOperation(VariableStore &store, YAML::const_iterator begin, YAML::const_iterator end) :
    Operation("bell", store, begin, end),
    state(0)
{
    enabled.setInteger(0);
}

void BellOperation::operator()(const AbstractBaseBuffer<HSV> &buffer) {
    const float perc = 0.10f;
    if (isEnabled()) {
        int mod = state % 60 == 0 ? 1 : 2;

        for (size_t i = 0; i < buffer.size(); i++) {
            if (int(i * perc) % mod == 0)
                buffer.at(i) = HSV{0.0f,1.0f, 1.0f};
            else
                 buffer.at(i) = HSV{0.0f,0.0f,0.0f};
        }
        state ++;
    } else {
        state = 0;
    }
}