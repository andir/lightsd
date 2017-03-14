//
// Created by andi on 3/14/17.
//

#ifndef LIGHTSD_RANDOM_H
#define LIGHTSD_RANDOM_H

#include <random>

template<typename Engine = std::mt19937>
// <std::ranlux48>, minstd_rand
inline int random_int_in_range(const int lower, const int upper) {

    if (lower >= upper) return lower;

    static std::random_device rd; // obtain a random number from hardware
    static Engine eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(lower, upper); // define the range

    const auto val = distr(eng);

    return val;
};

#endif //LIGHTSD_RANDOM_H
