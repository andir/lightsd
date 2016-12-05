//
// Created by andi on 12/4/16.
//

#include "SplashdropOperation.h"
#include <cassert>

SplashdropOperation::SplashdropOperation(VariableStore &store, YAML::const_iterator start, YAML::const_iterator end) :
    Operation("splashdrop", store, start, end)
{
}

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


void SplashdropOperation::draw(const AbstractBaseBuffer<HSV> &buffer) {

}

void SplashdropOperation::hitDrop(Drop& drop) {
    //TODO: add random colors
    drop.color = HSV{
            200,
            1.0f,
            1.0f
    };
    drop.decay_rate = 0.3f;
}

void SplashdropOperation::decayDrop(Drop &drop) {
    if (drop.color.value == 0.0f) {
        return;
    }

    drop.color.value *=  1.0f - drop.decay_rate;
}

HSV SplashdropOperation::drawDrop(Drop &drop, const size_t index, const AbstractBaseBuffer<HSV> &buffer) {
    if (drop.color.value <= 0.0f) {
        return buffer.at(index);
    }

    buffer.at(index) = drop.color;

    const size_t span = drop.color.value / drop.decay_rate;

    size_t left = span;
    size_t right = span;

    if (span >= index) {
        if (index > 0) {
            left = index - 1;
        } else {
            left = 0;
        }
    }

    if (index + span >= buffer.size()) {
        right = buffer.size() - index;
    }

    for (size_t i = 0; i < span; i++) {
        auto e = drop.color;
        e.value -= i * drop.decay_rate;
        if (i < left) {
            auto& b = buffer.at(index - i);
            b = e;
        }

        if (i < right) {
            auto& b = buffer.at(index + i);
            b = e;
        }
    }
    return buffer.at(index);
}


void SplashdropOperation::operator()(const AbstractBaseBuffer<HSV> &buffer) {
    if (!isEnabled())
        return;

    if (drops.size() != buffer.size()) {
        std::cerr << "resizing()" << std::endl;
        drops.clear();
        drops.resize(buffer.size());
        for (auto& drop : drops) {
            drop.color.value = 0;
        }
    }

    size_t i = 0;
    for (auto &drop : drops) {
        const float chance = 0.001;
        static const auto max_roll = 10000;
        const auto roll = random_in_range(0, max_roll);
        const auto bound = chance * max_roll;

        if (roll < bound) {
            hitDrop(drop);
            std::cout << "hit()" << i << std::endl;
        }

        drawDrop(drop, i++, buffer);
        decayDrop(drop);
    }
}



