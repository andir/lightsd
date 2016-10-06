//
// Created by andi on 10/6/16.
//

#ifndef LIGHTSD_FRAMESCHEDULER_H
#define LIGHTSD_FRAMESCHEDULER_H

#include "util.h"

class FrameScheduler {
    const int rate;
    uint64_t speed;

    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _stop;

public:
    FrameScheduler(const int rate) : rate(rate), speed(1000.0 / rate) {

    };

    void delay(const uint64_t duration) {
        const uint64_t diff = speed - duration;

        if (diff > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(diff));
        }
    }

};

class Frame {
    FrameScheduler& scheduler;
    MeasureTime measureTime;
public:
    Frame(FrameScheduler& scheduler) : scheduler(scheduler) {
    }

    ~Frame() {
        scheduler.delay(measureTime.measure());
    }

};


#endif //LIGHTSD_FRAMESCHEDULER_H
