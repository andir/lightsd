#include <thread>
#include <map>
#include <string>
#include "WorkerThread.h"
#include "net/MqttConnection.h"

#define MEASURE_TIME
#ifdef MEASURE_TIME
struct Measurment {
    uint64_t average;
    uint64_t max;
    uint64_t min;
    uint64_t count;
};


struct Timeing {
    std::map<std::string, struct Measurment> measurments;


    void report() {
        for (auto const &e : measurments) {
            std::string const &name = e.first;
            Measurment const &m = e.second;

            std::cerr << name << " average: " << m.average << " max: " << m.max << " min: " << m.min << " count: "
                      << m.count << std::endl;
        }
    }
};

class Measure {
    Timeing &timeing;
    struct Measurment &measurment;
    MeasureTime<std::chrono::nanoseconds> measureTime;
public:
    Measure(std::string name, Timeing &t) : timeing(t), measurment(t.measurments[name]) {
    }

    ~Measure() {
        auto delay = measureTime.measure();
        measurment.average += delay;
        measurment.count++;
        if (measurment.count == 1) {
            measurment.max = delay;
            measurment.min = delay;
        } else {
            measurment.average /= 2;
            if (uint64_t(delay) > measurment.max) {
                measurment.max = delay;
            } else if (uint64_t(delay) < measurment.min) {
                measurment.min = delay;
            }
        }
    }
};

#endif

WorkerThread::WorkerThread() : doRun(true), config_ptr(nullptr) {

}


void WorkerThread::setConfig(ConfigPtr cfg) {
    std::lock_guard<std::mutex> lock(config_mutex);
    new_config_ptr = std::move(cfg);
}


ConfigPtr WorkerThread::getConfig() {
    std::lock_guard<std::mutex> lock(config_mutex);
    if (new_config_ptr != nullptr) {
        return std::move(new_config_ptr);
    }
    return nullptr;
}

void WorkerThread::run() {
    while (doRun) {
#ifdef MEASURE_TIME
        Timeing t;
#endif
        ConfigPtr config = getConfig();

        while (config == nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            config = std::move(getConfig());
        };

        FrameScheduler scheduler(config->fps);
        Operation::BufferType buffer(std::make_unique<AllocatedBuffer<HSV> >(config->width));

#ifdef MEASURE_TIME
        unsigned int counter = 0;
#endif
        while (true) {
#ifdef MEASURE_TIME
            Measure("frame_time", t);
#endif
            if (new_config_ptr.get() != nullptr) {
                break;
            }

            Frame frame(scheduler);

            for (const auto &step : config->sequence) {
                step->update(config.get());
                if (step->isEnabled()) {
#ifdef MEASURE_TIME
                    Measure(step->getName(), t);
#endif
                    buffer = (*step)(buffer);
                }
            }
            for (const auto &output : config->outputs) {
                (*output.second).draw(*buffer);
            }
#ifdef MEASURE_TIME
            counter++;
            if (counter % (5 * config->fps) == 0) {
                t.report();
            }
#endif
        }
    }
}
