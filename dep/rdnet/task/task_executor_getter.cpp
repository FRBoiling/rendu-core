/*
* Created by boil on 2023/10/26.
*/

#include "task_executor_getter.h"
#include "timer/time_ticker.h"
#include "thread/thread_pool.h"
#include "thread/thread_helper.h"
#include "poller/event_poller.h"

RD_NAMESPACE_BEGIN

  TaskExecutor::Ptr TaskExecutorGetter::GetExecutor() {
    auto thread_pos = _thread_pos;
    if (thread_pos >= _threads.size()) {
      thread_pos = 0;
    }

    TaskExecutor::Ptr executor_min_load = _threads[thread_pos];
    auto min_load = executor_min_load->CalculateLoad();

    for (size_t i = 0; i < _threads.size(); ++i, ++thread_pos) {
      if (thread_pos >= _threads.size()) {
        thread_pos = 0;
      }

      auto th = _threads[thread_pos];
      auto load = th->CalculateLoad();

      if (load < min_load) {
        min_load = load;
        executor_min_load = th;
      }
      if (min_load == 0) {
        break;
      }
    }
    _thread_pos = thread_pos;
    return executor_min_load;
  }

  size_t TaskExecutorGetter::GetExecutorSize() const {
    return _threads.size();
  }

  void TaskExecutorGetter::GetExecutorDelay(const std::function<void(const std::vector<int> &)> &callback) {
    std::shared_ptr<std::vector<int> > delay_vec = std::make_shared<std::vector<int>>(_threads.size());
    std::shared_ptr<void> finished(nullptr, [callback, delay_vec](void *) {
      //此析构回调触发时，说明已执行完毕所有async任务
      callback((*delay_vec));
    });
    int index = 0;
    for (auto &th: _threads) {
      std::shared_ptr<Ticker> delay_ticker = std::make_shared<Ticker>();
      th->Async([finished, delay_vec, index, delay_ticker]() {
        (*delay_vec)[index] = (int) delay_ticker->ElapsedTime();
      }, false);
      ++index;
    }
  }

  std::vector<int32> TaskExecutorGetter::GetExecutorLoadVec() {
    std::vector<int32> vec(_threads.size());
    int i = 0;
    for (auto &executor: _threads) {
      vec[i++] = executor->CalculateLoad();
    }
    return vec;
  }


  void TaskExecutorGetter::for_each(const std::function<void(const TaskExecutor::Ptr &)> &cb) {
    for (auto &th: _threads) {
      cb(th);
    }
  }


  size_t TaskExecutorGetter::addPoller(const string &name, size_t size, int priority, bool register_thread,
                                       bool enable_cpu_affinity) {
    auto cpus = std::thread::hardware_concurrency();
    size = size > 0 ? size : cpus;
    for (size_t i = 0; i < size; ++i) {
      auto full_name = name + " " + std::to_string(i);
      auto cpu_index = i % cpus;
      EventPoller::Ptr poller(new EventPoller(full_name));
      poller->runLoop(false, register_thread);
      poller->Async([cpu_index, full_name, priority, enable_cpu_affinity]() {
        // 设置线程优先级
        ThreadPool::setPriority((ThreadPool::Priority) priority);
        // 设置线程名
        ThreadHelper::SetThreadName(full_name.data());
        // 设置cpu亲和性
        if (enable_cpu_affinity) {
          ThreadHelper::SetThreadAffinity(cpu_index);
        }
      });
      _threads.emplace_back(std::move(poller));
    }
    return size;
  }

RD_NAMESPACE_END