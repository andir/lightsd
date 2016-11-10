#ifndef LIGHTSD_RAINDROP_H
#define LIGHTSD_RAINDROP_H

#include <yaml-cpp/yaml.h>


#include "operations.h"
#include "VariableStore/VariableStore.h"
#include "VariableStore/BoundConcreteValueType.h"

class RaindropOperation : public Operation {

    BoundConcreteValue<int> hue_min;
    BoundConcreteValue<int> hue_max;

    BoundConcreteValue<float> saturation_min;
    BoundConcreteValue<float> saturation_max;


    BoundConcreteValue<float> value_min;
    BoundConcreteValue<float> value_max;

    BoundConcreteValue<float> chance;

    BoundConcreteValue<float> decay_low;
    BoundConcreteValue<float> decay_high;
    BoundConcreteValue<int> decay_resolution;

    struct Raindrop {
        HSV color;
        float decay_rate;

        void hit(const float decay_high, const float decay_low);
        void decay();
    };


    std::vector<Raindrop> leds;

    void hitRaindrop(Raindrop &drop);

public:
    RaindropOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end);

    void draw(const AbstractBaseBuffer<HSV>& buffer);

    void operator()(const AbstractBaseBuffer<HSV>& buffer);
};

#endif //LIGHTSD_RAINDROP_H