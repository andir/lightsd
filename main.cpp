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


template<typename ContainerType>
void initRainbow(ContainerType &data) {
    using ValueType = typename ContainerType::value_type;
    size_t steps = data.count();
    float stepDelta = 1.0f / steps;
    size_t i = 0;
    std::generate(data.begin(), data.end(), [stepDelta, &i]() {
        return algorithm::LinearColor<ValueType>(stepDelta * i++);
    });

}

template<typename ContainerType, typename ColorType>
void initSolidColor(ContainerType &data, ColorType color) {
    std::fill(data.begin(), data.end(), color);
};

template<typename Iterator, typename ColorType>
void initSolidColor(Iterator start, Iterator stop, ColorType color) {
    //HSV a = algorithm::convert(color);
//    std::generate(data.begin(), data.end(), [&color](){
//        return color;
//    });

    std::fill(start, stop, color);
};


template<typename A, typename B>
DuplicationOutput<A, B> duplicateOutput(A& a, B& b) {
    return DuplicationOutput<A, B>(a, b);
};

void test_leds() {
    FrameScheduler scheduler(60);


    const size_t num = 60 * 5;
    SharedMemoryBuffer shm("/tmp/test", num * sizeof(HSV));

    auto& buffer1 = *shm.get();

    DebugOutput o;
    UDPOutput u("172.23.97.143", "6453");
    WebsocketOutput wso1(9008);

    auto dup = duplicateOutput(u, wso1);
    auto output = FadingOutput<HSV, decltype(dup)>(scheduler, dup);

//    FadingOutput<HSV, DebugOutput> o(d);

    while (true) {
        for (size_t i = 0; i < num; i++) {
            const auto black = HSV{0.0f, 0.0f, 0.0f};
            initSolidColor(buffer1, black);

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



int main(int argn, const char *argv[]) {

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
            initRainbow<AllocatedBuffer<HSV>>(b);
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
//    auto output = duplicateOutput(dup1, fade);

    auto& output = fade;


    auto lastIteration = 0;

    while (1) {
        MeasureTime t1;

        const auto black = HSV{0.0f, 0.0f, 0.0f};
        initSolidColor(buffer1, black);

        const auto c1 = HSV{1.0f, 0.0f, 1.0f};
        const auto c2 = HSV{120.0f, 1.0f, 1.0f};
        const auto c3 = HSV{240.0f, 1.0f, 1.0f};
        initSolidColor(buffer1.begin(), buffer1.begin() + 10, c1);
        initSolidColor(buffer1.begin() + 10, buffer1.begin() + 20, c2);
        initSolidColor(buffer1.begin() + 20, buffer1.begin() + 30, c3);


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
