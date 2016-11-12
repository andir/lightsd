//
// Created by andi on 10/12/16.
//

#include <thread>
#include "WorkerThread.h"
#include "MqttVarStore.h"


WorkerThread::WorkerThread() : config_ptr(nullptr), doRun(true) {

}


void WorkerThread::setConfig(ConfigPtr cfg) {
    std::lock_guard <std::mutex> lock(config_mutex);
    new_config_ptr = std::move(cfg);
}


ConfigPtr WorkerThread::getConfig() {
    std::lock_guard <std::mutex> lock(config_mutex);
    if (new_config_ptr != nullptr) {
        return std::move(new_config_ptr);
    }
    return nullptr;
}

void WorkerThread::run() {
    while (doRun) {
        ConfigPtr config = getConfig();

        while (config == nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            config = std::move(getConfig());
        };

        FrameScheduler scheduler(config->fps);
        AllocatedBuffer<HSV> buffer(config->width);

        while (true) {
            if (new_config_ptr.get() != nullptr) {
                break;
            }

            Frame frame(scheduler);

            for (const auto &step : config->sequence) {
                (*step)(buffer);
            }
            for (const auto &output : config->outputs) {
                (*output.second).draw(buffer);
            }

        }
    }
}