#pragma once
#include <boost/thread/executors/basic_thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/thread/latch.hpp>
#include <boost/thread/future.hpp>
#include <deque>
#include <functional>

class JobQueue {
        std::mutex mutex;
        boost::latch latch;
        boost::basic_thread_pool pool;
        bool run;
public:
        JobQueue() : latch(0), run(true) {}
        inline void execute(const std::vector<std::function<void ()> > new_jobs) {
               std::unique_lock<std::mutex> l(mutex);
               latch.reset(new_jobs.size());
               for (auto& job : new_jobs) {
                        pool.submit(boost::bind(&JobQueue::_execute, this, job));
               }
               latch.wait();
        }

        inline void _execute(std::function<void ()> func) {
                func();
                latch.count_down();
        }
};
