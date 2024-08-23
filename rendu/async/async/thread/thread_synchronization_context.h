/*
* Created by boil on 2024/11/14.
*/

#ifndef RENDU_ASYNC_ASYNC_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H_
#define RENDU_ASYNC_ASYNC_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H_


#include "async_define.h"



#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <stdexcept>

RD_NAMESPACE_BEGIN

class ThreadSynchronizationContext
{
private:
  std::mutex mutex_;
  std::condition_variable condition_;
  std::queue<std::function<void()>> queue_;
  bool stop_;

public:
  ThreadSynchronizationContext() : stop_(false) {}

  ~ThreadSynchronizationContext()
  {
    stop();
  }

  void Update()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!stop_)
    {
      if (queue_.empty())
      {
        condition_.wait(lock);
      }
      else
      {
        auto action = queue_.front();
        queue_.pop();
        lock.unlock();
        try
        {
          action();
        }
        catch (const std::exception& e)
        {
          std::cerr << "Exception caught: " << e.what() << std::endl;
        }
        lock.lock();
      }
    }
  }

  void Post(std::function<void()> action)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (stop_)
    {
      throw std::runtime_error("Cannot post to a stopped ThreadSynchronizationContext");
    }
    queue_.push(action);
    condition_.notify_one();
  }

  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    stop_ = true;
    condition_.notify_all();
  }
};

//int main()
//{
//  ThreadSynchronizationContext context;
//
//  std::thread updateThread([&context]() {
//    context.Update();
//  });
//
//  context.Post([]() {
//    std::cout << "Hello from posted action!" << std::endl;
//  });
//
//  std::this_thread::sleep_for(std::chrono::seconds(1));
//
//  context.stop();
//  updateThread.join();
//
//  return 0;
//}

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H_
