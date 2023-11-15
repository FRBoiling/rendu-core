/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_TASK_EXAMPLE_H
#define RENDU_TASK_EXAMPLE_H


class CTask {
public:
  template<typename Func, typename... Args>
  CTask(Func &&func, Args &&...args)
      : m_future(std::async(std::launch::async, std::forward<Func>(func), std::forward<Args>(args)...)) {}

  template<typename Func>
  auto Await(Func &&func) {
    return ContinueWith([func = std::forward<Func>(func)](auto) mutable {
      return func();
    });
  }

  template<typename Func>
  auto ReportProgress(Func &&func) {
    return ContinueWith([func = std::forward<Func>(func)](auto) mutable {
      func();
      return nullptr;
    });
  }

  template<typename Func>
  auto Catch(Func &&func) {
    return ContinueWith([func = std::forward<Func>(func)](auto task) mutable {
      try {
        task.Result();
      } catch (const std::exception &e) {
        func(e);
      }
      return nullptr;
    });
  }

  template<typename Func>
  auto Run(Func &&func) {
    return StartNew([func = std::forward<Func>(func)]() mutable {
      func();
    });
  }

  template<typename Func>
  auto ContinueWith(Func &&func) {
    return CTask([fut = std::move(m_future), func = std::forward<Func>(func)]() mutable {
      try {
        return func(fut.get());
      } catch (const std::exception &e) {
        // Handle exception
        std::cout << "Exception: " << e.what() << '\n';
        throw;
      }
    });
  }

  void Result() {
    try {
      m_future.get();
    } catch (const std::exception &e) {
      // Handle exception
      std::cout << "Exception: " << e.what() << '\n';
      throw;
    }
  }

  bool IsCompleted() {
    return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
  }

  static void WhenAll(std::vector<CTask> &tasks) {
    for (auto &task: tasks) {
      task.Result();
    }
  }

  static size_t WhenAny(std::vector<CTask> &tasks) {
    while (true) {
      for (size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i].m_future.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready) {
          return i;
        }
      }
    }
  }

  static CTask Delay(int milliseconds) {
    return CTask([milliseconds]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    });
  }

  static CTask Run(std::function<void()> func) {
    return CTask(func);
  }

  static CTask StartNew(std::function<void()> func) {
    return CTask(func);
  }

private:
  std::future<void> m_future;
};

class CTaskWithCancellation : public CTask {
private:
  std::promise<void> m_promise;

public:
  CTaskWithCancellation(std::function<void(std::atomic<bool> &)> func)
      : CTask(CTask::Run([this, func]() {
          std::atomic<bool> cancelled{false};
          auto future = m_promise.get_future();
          while (future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
            func(cancelled);
            if (cancelled.load()) {
              break;
            }
          }
        })) {}

  void Cancel() {
    m_promise.set_value();
  }
};

class CTaskScheduler {
private:
  std::queue<CTask> m_tasks;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::atomic<bool> m_stop{false};
  std::vector<std::thread> m_threads;

public:
  CTaskScheduler(size_t numThreads) {
    m_threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
      m_threads.emplace_back([this]() {
        while (true) {
          std::unique_lock<std::mutex> lock(m_mutex);
          m_cv.wait(lock, [this]() { return m_stop.load() || !m_tasks.empty(); });
          if (m_stop.load() && m_tasks.empty()) {
            return;
          }
          CTask task = std::move(m_tasks.front());
          m_tasks.pop();
          lock.unlock();
          try {
            task.Result();
          } catch (const std::exception &e) {
            // Handle exception
            std::cout << "Exception: " << e.what() << '\n';
          }
        }
      });
    }
  }

  ~CTaskScheduler() {
    m_stop.store(true);
    m_cv.notify_all();
    for (auto &thread: m_threads) {
      if (thread.joinable()) {
        thread.detach();
      }
    }
  }

  void Schedule(CTask &&task) {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this](){ return m_tasks.size() < MAX_QUEUE_SIZE; });
      m_tasks.push(std::move(task));
    }
    m_cv.notify_one();
  }

private:
  static const size_t MAX_QUEUE_SIZE = 10000;
};

class TaskManager {
private:
  CTaskScheduler m_scheduler;

public:
  TaskManager(size_t numThreads) : m_scheduler(numThreads) {}

  template<typename Func, typename... Args>
  std::shared_ptr<CTaskWithCancellation> StartNew(Func&& func, Args&&... args) {
    auto task = std::make_shared<CTaskWithCancellation>(std::bind(std::forward<Func>(func), std::placeholders::_1, std::forward<Args>(args)...));
    m_scheduler.Schedule(CTask([task]() { task->Result(); }));
    return task;
  }
};

// 示例用法
int task_main() {
  TaskManager manager(4); // 创建一个有4个线程的任务管理器

  auto task = manager.StartNew([](std::atomic<bool>& cancelled) {
    if (!cancelled) {
      while (true){
        // 这里是你的任务代码
        std::cout << "Task is running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
    }
  });

  // 让任务运行一段时间
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // 取消任务
  task->Cancel();

  return 0;
}
#endif//RENDU_TASK_EXAMPLE_H
