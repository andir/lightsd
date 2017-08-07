//
// Created by andi on 10/12/16.
//

#ifndef LIGHTSD_WORKERTHREAD_H
#define LIGHTSD_WORKERTHREAD_H

#include <mutex>
#include "config.h"
#include "FrameScheduler.h"

class WorkerThread {
    bool doRun;
    ConfigPtr config_ptr;
    ConfigPtr new_config_ptr;
    std::mutex config_mutex;

public:
    WorkerThread();

    void setConfig(ConfigPtr cfg);

    ConfigPtr getConfig();

    void run();
};


#endif //LIGHTSD_WORKERTHREAD_H
