/*
* Created by boil on 2023/10/26.
*/

#include "task_executor_getter.h"
#include "thread/thread_helper.h"
#include "time/time_ticker.h"

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


RD_NAMESPACE_END