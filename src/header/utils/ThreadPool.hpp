#pragma once

// buat threadpool
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// penerimaan task dengan berbagai tipe
#include <future>
#include <functional>
#include <type_traits>
#include <stdexcept>

using namespace std;


// Based on geeksforgeeks.org
class ThreadPool
{
public:
    ThreadPool(size_t threadCount) : stop_(false)
    {
        // workers thread
        for (size_t i = 0; i < threadCount; ++i)
        {
            workers.emplace_back([this]() {
                while (true)
                {
                    function<void()> task;
                    {
                        // lock queue biar data bisa diakses dengan kebersamaan (bruh)
                        unique_lock<mutex> lock(queueMutex);
                        condition_.wait(lock, [this]() {
                            return stop_ || !tasks_.empty();
                        });
                        

                        // thread exit kalau stop true dan tidak ada task lainnya
                        if (stop_ && tasks_.empty())
                        {
                            return;
                        }

                        task = move(tasks_.front());
                        tasks_.pop();
                    }

                    task();
                }
            });
        }
    }

    ~ThreadPool()
    {
        {
            // lock queue
            unique_lock<mutex> lock(queueMutex);
            stop_ = true;
        }

        condition_.notify_all();

        for (auto &worker : workers)
        {
            worker.join();
        }
    }

    // thread pool queue
    template <class F>
    auto enqueue(F&& f) -> future<typename invoke_result<F>::type>
    {
        // memperhatikan tipe yang dibalikkin
        using ReturnType = typename invoke_result<F>::type;

        auto taskPtr = make_shared<packaged_task<ReturnType()>>(
            forward<F>(f)
        );

        future<ReturnType> result = taskPtr->get_future();

        {
            // lock biar tidak saling nimpa data
            lock_guard<mutex> lock(queueMutex);
            
            // ts error handling biasa
            if (stop_)
            {
                throw runtime_error("[RTE] enqueue on stopped.");
            }

            tasks_.emplace([taskPtr]() {
                (*taskPtr)();
            });
        }

        condition_.notify_one();
        return result;
    }

private:
    vector<thread> workers;
    queue<function<void()>> tasks_;

    mutex queueMutex;
    condition_variable condition_;
    bool stop_;
};