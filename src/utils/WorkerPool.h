#pragma once
#include <deque>
#include <thread>
#include <functional>

class JobQueue {
        std::condition_variable done_cv;
        std::condition_variable work_cv;
        std::deque<std::function<void ()>> jobs;
        std::mutex mutex;
        size_t counter;
        bool run;
public:
        JobQueue() : counter(0), run(true) {}
        inline void submit(const std::vector<std::function<void ()> > new_jobs) {
               std::unique_lock<std::mutex> l(mutex);
               jobs.insert(jobs.end(), new_jobs.begin(), new_jobs.end());
               counter = jobs.size();
               if (counter > 1)
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
               return job;
        }

        void done() {
                std::unique_lock<std::mutex> l(mutex);
                counter--;
                if (counter == 0) {
                        done_cv.notify_all();
                }
        }

        inline void wait() {
                std::unique_lock<std::mutex> l(mutex);
                //std::cerr << "queue size: " << jobs.size() << std::endl;
                done_cv.wait(l, [this] {return counter == 0;});
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
