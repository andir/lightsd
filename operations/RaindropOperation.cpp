#include "RaindropOperation.h"

#include <cassert>
#include "../utils/random.h"


RaindropOperation::RaindropOperation(VariableStore
                                     &store,
                                     YAML::const_iterator start, YAML::const_iterator
                                     end) :
        Operation("raindrop", store, start, end),

        hue_min("raindrop/hue_min", Operation::HSV_HUE, store, getValueByKey<float>("hue_min", start, end, 0.0f)),
        hue_max("raindrop/hue_max", Operation::HSV_HUE, store, getValueByKey<float>("hue_max", start, end, 360.0f)),

        saturation_min("raindrop/saturation_min", Operation::HSV_SATURATION, store,
                       getValueByKey<float>("saturation_min", start, end, 0.0f)),
        saturation_max("raindrop/saturation_max", Operation::HSV_SATURATION, store,
                       getValueByKey<float>("saturation_max", start, end, 1.0f)),

        value_min("raindrop/value_min", Operation::HSV_VALUE, store,
                  getValueByKey<float>("value_min", start, end, 0.8f)),
        value_max("raindrop/value_max", Operation::HSV_VALUE, store,
                  getValueByKey<float>("value_max", start, end, 1.0f)),


        chance("raindrop/chance", Operation::FLOAT_0_1, store, getValueByKey<float>("chance", start, end, 0.95f)),

        decay_low("raindrop/decay_low", Operation::FLOAT_0_1, store,
                  getValueByKey<float>("decay_low", start, end, 0.8f)),
        decay_high("raindrop/decay_high", Operation::FLOAT_0_1, store,
                   getValueByKey<float>("decay_high", start, end, 0.5f)),
        decay_resolution("raindrop/decay_resolution", Operation::FLOAT_0_1, store,
                         getValueByKey<float>("decay_resolution", start, end, 1.0f)) {

}


void RaindropOperation::hitRaindrop(Raindrop &drop) {
    const int saturation = random_int_in_range(saturation_min.getValue() * 1000, saturation_max.getValue() * 1000);
    drop.color.hue = random_int_in_range(hue_min.getValue(), hue_max.getValue());
    drop.color.saturation = float(saturation) / 1000.0f;
    drop.color.value =
            float(random_int_in_range(value_min.getValue() * 10000, value_max.getValue() * 10000)) / 10000.0f;

    const auto decay_rate = float(random_int_in_range(
            decay_low.getValue() * decay_resolution.getValue() * 10000,
            decay_high.getValue() * decay_resolution.getValue() * 10000
    )) / (10000.0f * decay_resolution.getValue());

    drop.decay_rate = decay_rate;
}

void RaindropOperation::Raindrop::decay() {
    const auto factor = std::max(std::min(decay_rate, 1.0f), 0.0f);
    this->color.value *= factor;
}

Operation::BufferType RaindropOperation::operator()(Operation::BufferType &buffer) {
    if ((*buffer).size() != leds.size()) {
        leds.clear();
        leds.resize((*buffer).size());
        for (auto &l : leds) {
            l.color.value = 0.0f;
        }
    }

    auto it = leds.begin();
    for (auto &led : *buffer) {
        assert(it != leds.end());
        auto &drop = *it++;
        static const auto max_roll = 1000000;
        const auto roll = random_int_in_range(0, max_roll);
        const auto bound = (1 - chance.getValue()) * max_roll;

        if (roll >= bound) {
            hitRaindrop(drop);
        }

        led = drop.color;
        drop.decay();
    }
    return buffer;
}
