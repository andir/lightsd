#include "RaindropOperation.h"

#include <cassert>
#include <random>

template<typename Engine = std::mt19937>
// <std::ranlux48>, minstd_rand
inline int random_in_range(const int lower, const int upper) {

    if (lower >= upper) return lower;

    static std::random_device rd; // obtain a random number from hardware
    static Engine eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(lower, upper); // define the range

    const auto val = distr(eng);

    return val;
};


RaindropOperation::RaindropOperation(VariableStore
                                     &store,
                                     YAML::const_iterator start, YAML::const_iterator
                                     end) :
        Operation("raindrop", store, start, end),

        hue_min("raindrop/hue_min", Operation::HSV_HUE, store, getValueByKey<int>("hue_min", start, end, 0.0f)),
        hue_max("raindrop/hue_max", Operation::HSV_HUE, store, getValueByKey<int>("hue_max", start, end, 360.0f)),

        saturation_min("raindrop/saturation_min", Operation::HSV_SATURATION, store, getValueByKey<float>("saturation_min", start, end, 0.0f)),
        saturation_max("raindrop/saturation_max", Operation::HSV_SATURATION, store, getValueByKey<float>("saturation_max", start, end, 1.0f)),

        value_min("raindrop/value_min", Operation::HSV_VALUE, store, getValueByKey<float>("value_min", start, end, 0.8f)),
        value_max("raindrop/value_max", Operation::HSV_VALUE, store, getValueByKey<float>("value_max", start, end, 1.0f)),


        chance("raindrop/chance", Operation::FLOAT_0_1, store, getValueByKey<float>("chance", start, end, 0.95f)),

        decay_low("raindrop/decay_low", Operation::FLOAT_0_1,  store, getValueByKey<float>("decay_low", start, end, 0.8f)),
        decay_high("raindrop/decay_high", Operation::FLOAT_0_1, store, getValueByKey<float>("decay_high", start, end, 0.5f)),
        decay_resolution("raindrop/decay_resolution", Operation::FLOAT_0_1, store,
                         getValueByKey<float>("decay_resolution", start, end, 1.0f)) {

}


void RaindropOperation::hitRaindrop(Raindrop &drop) {

    const int saturation = random_in_range(saturation_min.getFloat() * 1000, saturation_max.getFloat() * 1000);
    drop.color.hue = random_in_range(hue_min.getInteger(), hue_max.getInteger()) * 360.0f;
    drop.color.saturation = float(saturation) / 1000.0f;
    drop.color.value = float(random_in_range(value_min.getFloat() * 10000, value_max.getFloat() * 10000)) / 10000.0f;

    const auto decay_rate = float(random_in_range(
            decay_low.getFloat() * decay_resolution.getInteger() * 10000,
            decay_high.getFloat() * decay_resolution.getInteger() * 10000
    )) / (10000.0f * decay_resolution.getInteger());

    drop.decay_rate = decay_rate;
}

void RaindropOperation::Raindrop::decay() {
    const auto factor = std::max(std::min(decay_rate, 1.0f), 0.0f);
    this->color.value *= factor;
}

void RaindropOperation::operator()(const AbstractBaseBuffer<HSV> &buffer) {
    if (!isEnabled())
        return;

    if (buffer.size() != leds.size()) {
        leds.clear();
        leds.resize(buffer.size());
        for (auto &l : leds) {
            l.color.value = 0.0f;
        }
    }

    auto it = leds.begin();
    for (auto &led : buffer) {
        assert(it != leds.end());
        auto &drop = *it++;
        static const auto max_roll = 10000;
        const auto roll = random_in_range(0, max_roll);
        const auto bound = chance.getFloat() * max_roll;

        if (roll >= bound) {
            hitRaindrop(drop);
        }

        led = drop.color;
        drop.decay();
    }
}
