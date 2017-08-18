#pragma once
#include <boost/thread/latch.hpp>
#include <deque>
#include <thread>
#include <functional>

class JobQueue {
        std::condition_variable work_cv;
        std::deque<std::function<void ()>> jobs;
        std::mutex mutex;
        std::atomic<size_t> counter;
        boost::latch latch;
        bool run;
public:
        JobQueue() : counter(0), latch(0), run(true) {}
        inline void submit(const std::vector<std::function<void ()> > new_jobs) {
               std::unique_lock<std::mutex> l(mutex);
               jobs.insert(jobs.end(), new_jobs.begin(), new_jobs.end());
               const auto count = jobs.size();
               latch.reset(count);
               if (count > 1)
                       work_cv.notify_all();
               else {
                       work_cv.notify_one();
               }
        }

        inline std::function<void ()> pop() {
               std::unique_lock<std::mutex> l(mutex);
               work_cv.wait(l, [this]() -> bool { return (jobs.size() > 0) || run == false; });
               if (run == false) {
                       // FIXME: hack to yield empty work on exit
                       return std::function([]{});
               }
               auto job = jobs.front();
               jobs.pop_front();
               return std::move(job);
        }

        void done() {
                latch.count_down();       
        }

        inline void wait() {
                latch.wait();
        }

        void stop() {
                std::unique_lock<std::mutex> l(mutex);
                run = false;
                work_cv.notify_all();
        }
};

class WorkerPool {
        JobQueue queue;
        std::vector<std::thread> workers;
        bool run;
public:
        WorkerPool(size_t num = 0) : run(true) {
                if (num == 0) {
                        num = std::thread::hardware_concurrency();
                        if (num == 0) {
                                num = 2;
                        }
                }
                for (size_t i =0; i < num; i++) {
                        workers.push_back(std::move(std::thread([this]{
                                while(run) {
                                       auto work = queue.pop();
                                       work();
                                       queue.done();
                                }
                        })));
                }
        }
        ~WorkerPool() {
                run = false;
                queue.stop();
                for (auto& worker : workers) {
                        if (worker.joinable()){
                                worker.join();
                        }
                }
        }

        void submit(std::vector<std::function<void ()>> jobs) {
                queue.submit(jobs);
        }

        void wait() {
                queue.wait();
        }
};
