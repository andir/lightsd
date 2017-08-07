#include <thread>
#include "FrameScheduler.h"


FrameScheduler::FrameScheduler(const int rate) : rate(rate), speed(1000.0 / rate) {

}

void FrameScheduler::delay(const uint64_t duration) {
    const uint64_t diff = speed - duration;

    if (diff > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(diff));
    }
}