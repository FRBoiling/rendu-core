/*
* Created by boil on 25-4-9.
*/

#ifndef CONCURRENTQUEUE_HPP
#define CONCURRENTQUEUE_HPP

#include <queue>

//=============== 线程安全队列 ===============//
namespace concurrency{

template <typename T>
class ConcurrentQueue
{
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cv;

public:
    void push(T&& item)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(item));
        }
        cv.notify_one();
    }

    bool try_pop(T& item)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (cv.wait_for(lock, std::chrono::milliseconds(10),
                        [this] { return !queue.empty(); }))
        {
            item = std::move(queue.front());
            queue.pop();
            return true;
        }
        return false;
    }

    size_t size();
};
}
#endif //CONCURRENTQUEUE_HPP
