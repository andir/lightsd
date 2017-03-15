//
// Created by andi on 10/7/16.
//

#include <csignal>
#include <thread>
#include <stdlib.h>
#include <iostream>

#include "config.h"
#include "WorkerThread.h"


static WorkerThread worker_thread;
static std::string config_filename;


void sigHupHandler(int signum) {
    if (signum == SIGHUP) {
        static std::mutex m;
        std::unique_lock<std::mutex> lock(m);
        try {
                auto conf = parseConfig(config_filename);
                worker_thread.setConfig(std::move(conf));
        } catch(ConfigParsingException& e) {
                std::cerr << "Failed to parse configuration: " << e.what() << std::endl;
        }
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
        worker_thread.run();
        return 0;
    });

    signal(SIGHUP, sigHupHandler);

    t.join();

    return 0;
}
