//
// Created by andi on 10/7/16.
//

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <boost/algorithm/string.hpp>

#include <stdlib.h>
#include "buffer.h"
#include "hsv.h"
#include "algorithm.h"
#include "FrameScheduler.h"
#include "Output.h"
#include "WebsocketOutput.h"
#include "operations.h"
#include "MqttVarStore.h"
#include "config.h"











class WorkerThread {
    bool doRun;
    ConfigPtr config_ptr;
    ConfigPtr new_config_ptr;
    std::mutex config_mutex;

public:
    WorkerThread() : config_ptr(nullptr), doRun(true) {

    }

    void setConfig(ConfigPtr cfg) {
        std::lock_guard<std::mutex> lock(config_mutex);
        new_config_ptr = std::move(cfg);
    }

    ConfigPtr getConfig() {
        std::lock_guard<std::mutex> lock(config_mutex);
        if (new_config_ptr != nullptr) {
            return std::move(new_config_ptr);
        }
        return nullptr;
    }


    void run() {
        while (doRun) {
            ConfigPtr config = getConfig();

            while (config == nullptr) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                config = std::move(getConfig());
            };

            FrameScheduler scheduler(config->fps);
            AllocatedBuffer<HSV> buffer(config->width);
            MqttVarStore mqtt(config->store, "whisky.trollhoehle.net", "/test/");



            for (const auto& key : config->store.keys() ) {
                std::cout << key << " " << *config->store.getVar(key) << std::endl;
            }

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
};



static WorkerThread worker_thread;
static std::string config_filename;


void sigHupHandler(int signum) {
    if (signum == SIGHUP) {
        static std::mutex m;
        std::unique_lock<std::mutex> lock(m);
        auto conf = parseConfig(config_filename);
        worker_thread.setConfig(std::move(conf));
    } else {
        exit(signum);
    }
}

int main(const int argc, const char *argv[]) {
    config_filename = "config.yml";

    if (argc > 1) {
        config_filename = std::string(argv[1]);
    }

    worker_thread.setConfig(parseConfig(config_filename));


    std::thread t([]() {
//        sigset_t set;
//        sigemptyset(&set);
//        sigprocmask(SIG_SETMASK, &set, NULL);
//        if (pthread_sigmask(SIG_SETMASK, &set, NULL)) {
//            std::cerr << "failed to set SIGHUP mask" << std::endl;
//            return 1;
//        }
        worker_thread.run();
        return 0;
    });

    signal(SIGHUP, sigHupHandler);

    t.join();

    return 0;
}