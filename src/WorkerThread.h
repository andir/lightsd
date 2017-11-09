#pragma once

#include "FrameScheduler.h"
#include "config.h"
#include "utils/JobQueue.h"
#include <mutex>

class WorkerThread {
  bool doRun;
  ConfigPtr config_ptr;
  ConfigPtr new_config_ptr;
  std::mutex config_mutex;
  JobQueue job_queue;

 public:
  WorkerThread();

  void setConfig(ConfigPtr cfg);

  ConfigPtr getConfig();

  void run();
};
