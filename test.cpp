#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <fstream>
#include <thread>
#include <tgmath.h>

#include "util.h"

#include "hsv.h"
#include "rgb.h"
#include "buffer.h"
#include "algorithm.h"
#include "SharedMemoryBuffer.h"
#include "DebugOutput.h"
#include "UDPOutput.h"
#include "FadingOutput.h"
#include "WebsocketOutput.h"
#include "DuplicationOutput.h"
#include "bounded_type.h"
#include "FrameScheduler.h"





template<typename A, typename B>
DuplicationOutput<A, B> duplicateOutput(A& a, B& b) {
    return DuplicationOutput<A, B>(a, b);
};

void test_leds() {
    FrameScheduler scheduler(60);


    const size_t num = 60 * 5;
    SharedMemoryBuffer shm("/tmp/test", num * sizeof(HSV));

    auto& buffer1 = *shm.get();

//    DebugOutput o;
    UDPOutput u("172.23.97.143", "6453");
    WebsocketOutput wso1(9008);

    auto dup = duplicateOutput(u, wso1);
    auto output = FadingOutput<HSV, decltype(dup)>(scheduler, dup);

//    FadingOutput<HSV, DebugOutput> o(d);

    while (true) {
        for (size_t i = 0; i < num; i++) {
            const auto black = HSV{0.0f, 0.0f, 0.0f};
            algorithm::initSolidColor(buffer1, black);

            const std::vector<HSV> c = std::vector<HSV>({
                                                          HSV{120.0f, 1.0f, 1.0f},
                                                          HSV{200.0f, 1.0f, 1.0f},
                                                          HSV{350.0f, 1.0f, 1.0f},
                                                          HSV{1.0f, 0.0f, 1.0f},
                                                  });
            for (auto& color : c) {
                buffer1.at(i) = color;

                if (i > 0)
                    buffer1.at(i-1) = HSV{100.0f, 0.5f, 1.0f};
                if (i < num)
                    buffer1.at(i+1) = HSV{200.0f, 0.5f, 1.0f};

                output.draw(buffer1);
            }

        }
    }

}


void flokli() {
    const size_t num = 100;
    AllocatedBuffer<HSV> buffer(num);

    // Setup Outputs
    WebsocketOutput websocketOutput1(9000);

    // Create a FrameScheduler to ensure 60 FPS
    FrameScheduler scheduler(60);

    // Setup Frame Fading output
    // It will fade frames and then pass them on to the websocket
    FadingOutput<HSV, WebsocketOutput> fadingOutput(scheduler, websocketOutput1);

    // loop counter for rotation of rainbow
    size_t counter = 0;

    // Loop forever
    while (1) {
        // Init buffer with rainbow
        algorithm::initRainbow(buffer);

        size_t buffer_offset = counter % num;

        // per tick rotate by 1, will reset after `num` steps.
        std::rotate(&buffer.at(0), &buffer.at(0) + buffer_offset, &*buffer.end());


        // pass our frame onto the output
        // the fading output is `special` it does framing
        fadingOutput.draw(buffer);


        // increment counter
        counter ++;
    }

}


int main_old(int argn, const char *argv[]) {

    if (argn > 2) {
        test_leds();
        return 0;
    }

    const static size_t num = 65 * 5;
    FrameScheduler scheduler(30);

    SharedMemoryBuffer shm("/tmp/test", num * sizeof(HSV));

    struct {
        AllocatedBuffer<HSV> create() {
            auto b = AllocatedBuffer<HSV>(num);
            algorithm::initRainbow<AllocatedBuffer<HSV>>(b);
            return b;
        }
    } factory;

    // move constructed, just testing :D
    auto &buffer1 = *shm.get();


    std::vector<float> partialMask((num / 2) - 1);
    std::generate(partialMask.begin(), partialMask.end(), []() {
        return 0.1f;
    });


    UDPOutput u("172.23.97.143", "6453");

    MeasureTime clock;
    WebsocketOutput wso1(9000);
    WebsocketOutput wso2(9003);




    auto out = duplicateOutput(u, wso1);
    FadingOutput<HSV, decltype(out)> fade(scheduler, out);
    auto& output = fade;


    auto lastIteration = 0;

    while (1) {
        MeasureTime t1;

        const auto black = HSV{0.0f, 0.0f, 0.0f};
        algorithm::initSolidColor(buffer1, black);

        const auto c1 = HSV{1.0f, 0.0f, 1.0f};
        const auto c2 = HSV{120.0f, 1.0f, 1.0f};
        const auto c3 = HSV{240.0f, 1.0f, 1.0f};
        algorithm::initSolidColor(buffer1.begin(), buffer1.begin() + 10, c1);
        algorithm::initSolidColor(buffer1.begin() + 10, buffer1.begin() + 20, c2);
        algorithm::initSolidColor(buffer1.begin() + 20, buffer1.begin() + 30, c3);


        // restrict power consumption by reducing total values down to 40%
        algorithm::MaskBuffer(0.4f, buffer1);
        const size_t steps = 150;
        size_t tick = (clock.measure() / 10) % (2 * steps);

        if (tick > steps) {
            tick = steps - (tick - steps);
        }

        std::rotate(&buffer1.at(0), &buffer1.at(1) + int((tick * (double(num) / double(steps))) - 1), &*buffer1.end());


        const auto shade = 0.15f + ((0.9f / steps) * tick);
        //std::cout << "tick: " << tick << " shade:" << shade << std::endl;
        algorithm::MaskBuffer(shade, buffer1);

        //algorithm::PartialMaskBuffer(counter % 2 == 0 ? 0.8f : 0.5f, buffer1.begin() + (num / 4), buffer1.begin() + (3 * num /4));
//30
        const size_t offset = num / 10;
//
//        algorithm::Fade(buffer1.begin(), buffer1.begin() + offset);
//        algorithm::Fade(buffer1.rbegin(), buffer1.rbegin() + offset);
//
//

        //u.draw(buffer1);


        output.draw(buffer1);
        lastIteration += t1.measure();
    }

    return 0;
}
