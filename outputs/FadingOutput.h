#ifndef LIGHTSD_FADINGOUTPUT_H
#define LIGHTSD_FADINGOUTPUT_H

#include "../utils/util.h"
#include "../FrameScheduler.h"

#include <algorithm>

template<typename ColorModel, typename ChildT>
class FadingOutput {
    FrameScheduler& scheduler;

    std::vector<ColorModel> lastFrame;
    std::vector<ColorModel> diff;

    ChildT& child;
public:

    FadingOutput(FrameScheduler& scheduler, ChildT& child) : scheduler(scheduler), child(child) {}

    void draw(const std::vector<HSV>& buffer) {
        _draw(buffer);
    }
    void draw(const AbstractBaseBuffer<HSV>& buffer) {
        _draw(buffer);
    }
private:
    template<typename Container>
    void _draw(const Container& buffer) {
        if (lastFrame.empty()) {
            lastFrame.resize(buffer.count());
            auto ot = lastFrame.begin();
            for (auto it = buffer.begin(); it != buffer.end(); it++, ot++)
                *ot = *it;

            child.draw(lastFrame);

            return;
        }

        if (buffer.count() != lastFrame.size()) {// buffer size change?!
            lastFrame.resize(buffer.count());
        }

        if (diff.size() != lastFrame.size()) {
            diff.resize(lastFrame.size());
        }

        const size_t steps = 10;


       // diff = old - new


        std::transform(lastFrame.begin(), lastFrame.end(), buffer.begin(), diff.begin(),
                       [steps](const auto& lastf, const auto& newf){
//                           auto diff = HSV{
//                                   (newf.hue - lastf.hue) / steps,
//                                   (newf.saturation - lastf.saturation) / steps,
//                                   (newf.value - lastf.value) / steps
//                           };
                           const auto diff = (newf - lastf);
                           const auto delta = diff / steps;

                           //std::cout << "( " << newf.string() << " - " << lastf.string() << ") / " << steps << " = " << diff.string();
                           //std::cout << diff.string() << "\t" << newf.string() << " " << lastf.string() << "\n";
                           return delta;


//            if (newf.hue > lastf.hue) {
//                return (newf - lastf) / steps;
//            } else {
//                return (lastf - newf) / steps;
//            }

        });

        for (size_t i = 0 ; i < steps; i++) {
            Frame frame(scheduler);
            MeasureTime t1;
            size_t counter = 0;
            std::transform(lastFrame.begin(), lastFrame.end(), diff.begin(), lastFrame.begin(), [this, &buffer, &i, &counter](auto& lastf, const auto& difff){
                const auto f = lastf + difff;
//                std::cout << "(" << i << ")" << lastf.string() << " + " << difff.string() << " = " << f.string() << std::endl;
//                std::cout << "target: " << buffer.at(counter).string() << std::endl;
//                counter ++;
//                assert(f.hue >= 0 && f.hue <= 360);
//                assert(f.saturation >= 0 && f.saturation <= 1);
//                assert(f.value >= 0 && f.value <= 1);
//                const auto rgb = f.toRGB();
//                if (counter > 0) {
//                    volatile const auto target = buffer.at(counter);
//                    volatile const auto ptarget = buffer.at(counter - 1);
//
//                    const auto prev = lastFrame.at(counter -1);
//                    const float phue = prev.hue;
//
////                    if (phue > f.hue) {
////                        assert(phue - f.hue <= 24.0f);
////                    } else {
////                        assert(f.hue - phue <= 24.0f);
////                    }
//                }
                return f;
            });
//            std::cout << "\t----------" << std::endl;
            child.draw(lastFrame);
            //std::this_thread::sleep_for(std::chrono::milliseconds((1000 / 120) - t1.measure()));
        }
    }
};


#endif //LIGHTSD_FADINGOUTPUT_H
