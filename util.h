//
// Created by andi on 10/6/16.
//

#ifndef LIGHTSD_UTIL_H
#define LIGHTSD_UTIL_H

#include <chrono>

struct MeasureTime {

    const std::chrono::steady_clock::time_point start;

    MeasureTime() : start(std::chrono::steady_clock::now()) {}

    /*
     * time in ms
     */
    auto measure() {

        const auto p = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(
                p - start
        ).count();

    }

    ~MeasureTime() {}
};


#endif //LIGHTSD_UTIL_H
