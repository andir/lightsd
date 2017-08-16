#include <thread>
#include <map>
#include <string>
#include "WorkerThread.h"
#include "net/MqttConnection.h"

#define MEASURE_TIME
#ifdef MEASURE_TIME
struct Measurement {
    uint64_t average;
    uint64_t max;
    uint64_t min;
    uint64_t count;

    void reset() {
        average = 0;
        max = 0;
        min = 0;
        count = 0;
    }
};


struct Timing {
    std::map<std::string, struct Measurement> measurements;

    void report() {
        for (auto &e : measurements) {
            std::string const &name = e.first;
            Measurement &m = e.second;

            std::cerr << name << " average: " << m.average << " max: " << m.max << " min: " << m.min << " count: "
                      << m.count << std::endl;
            m.reset();
        }
    }
};

class Measure {
    Timing &timing;
    struct Measurement &measurement;
    MeasureTime<std::chrono::nanoseconds> measureTime;
public:
    Measure(const std::string& name, Timing &t) : timing(t), measurement(t.measurements[name]) {
    }

    ~Measure() {
        auto delay = measureTime.measure();
        measurement.average += delay;
        measurement.count++;
        if (measurement.count == 1) {
            measurement.max = delay;
            measurement.min = delay;
        } else {
            measurement.average /= 2;
            if (uint64_t(delay) > measurement.max) {
                measurement.max = delay;
            } else if (uint64_t(delay) < measurement.min) {
                measurement.min = delay;
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
        Timing t;
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
            Measure frame_time("frame_time", t);
#endif
            if (new_config_ptr.get() != nullptr) {
                break;
            }

            Frame frame(scheduler);

            {
                    Measure update_time("update_time", t);
                    // Schedule async tasks for each of the plugins update() function
                    std::vector<std::function<void ()> > jobs;
                    jobs.reserve(config->sequence.size());
                    for (const auto &step : config->sequence) {
                            auto job = [&step,&config]() -> void { 
                               step->update(config->width, config->fps);
                               return;
                            };
                            jobs.push_back(std::move(job));
                    }
                    worker_pool.submit(jobs);
                    worker_pool.wait();
            }


            for (const auto &step : config->sequence) {
                //step->update(config->width, config->fps);
                if (step->isEnabled()) {
#ifdef MEASURE_TIME
                    Measure step_time(step->getName(), t);
#endif
                    buffer = (*step)(buffer);
                }
            }
            for (const auto &output : config->outputs) {
                Measure output_time(output.first, t);
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
