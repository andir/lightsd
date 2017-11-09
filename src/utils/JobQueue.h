#pragma once

#ifdef PARALLEL_UPDATE
#include <boost/thread.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/latch.hpp>
#include <deque>
#endif
#include <functional>

#include "../operations/Operation.h"

class JobQueue {
#ifdef PARALLEL_UPDATE
  std::mutex mutex;
  boost::latch latch;
  boost::basic_thread_pool pool;
#endif
 public:
  JobQueue()
#ifdef PARALLEL_UPDATE
      : latch(0)
#endif
  {
  }
  template <typename OpT>
  inline void submit(const std::vector<OpT>& ops,
                     const size_t width,
                     const size_t fps) {
#ifdef PARALLEL_UPDATE
    std::unique_lock<std::mutex> l(mutex);
    latch.reset(ops.size());
#endif
    for (const auto& op : ops) {
      Operation* const o = op.get();
#ifdef PARALLEL_UPDATE
      pool.submit(std::bind(&JobQueue::_execute, this, o, width, fps));
#else
      o->update(width, fps);
#endif
    }
  }

  template <typename OpT>
  inline void execute(const std::vector<OpT>& ops,
                      const size_t width,
                      const size_t fps) {
    submit(ops, width, fps);
#ifdef PARALLEL_UPDATE
    wait();
#endif
  }

  inline void wait() {
#ifdef PARALLEL_UPDATE
    latch.wait();
#endif
  }

#ifdef PARALLEL_UPDATE
  inline void _execute(Operation* const op,
                       const size_t width,
                       const size_t fps) {
    op->update(width, fps);
    latch.count_down();
  }
#endif
};
