/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_QUEUE_ATOMIC_PRIORITY_QUEUE_H_
#define RENDU_ASYNC_ASYNC_QUEUE_ATOMIC_PRIORITY_QUEUE_H_

#include "async_define.h"
#include "queue_object.h"
#include "lock/lock_include.h"

RD_NAMESPACE_BEGIN

template<typename T>
class AtomicPriorityQueue : public QueueObject {
public:
  AtomicPriorityQueue() = default;

  /**
     * 尝试弹出
     * @param value
     * @return
     */
  Bool tryPop(T& value) {
    Bool result = false;
    if (mutex_.try_lock()) {
      if (!priority_queue_.empty()) {
        value = std::move(*priority_queue_.top());
        priority_queue_.pop();
        result = true;
      }
      mutex_.unlock();
    }

    return result;
  }


  /**
     * 尝试弹出多个任务
     * @param values
     * @param maxPoolBatchSize
     * @return
     */
  Bool tryPop(std::vector<T>& values, int maxPoolBatchSize) {
    Bool result = false;
    if (mutex_.try_lock()) {
      while (!priority_queue_.empty() && maxPoolBatchSize-- > 0) {
        values.emplace_back(std::move(*priority_queue_.top()));
        priority_queue_.pop();
        result = true;
      }
      mutex_.unlock();
    }

    return result;
  }


  /**
     * 传入数据
     * @param value
     * @param priority 任务优先级，数字排序
     * @return
     */
  Void push(T&& value, int priority) {
    std::unique_ptr<T> task(make_unique<T>(std::move(value), priority));
    RD_LOCK_GUARD lk(mutex_);
    priority_queue_.push(std::move(task));
  }


  /**
     * 判定队列是否为空
     * @return
     */
  Bool empty() {
    RD_LOCK_GUARD lk(mutex_);
    return priority_queue_.empty();
  }

  RD_NON_COPYABLE(AtomicPriorityQueue)

private:
  std::priority_queue<std::unique_ptr<T> > priority_queue_;    // 优先队列信息，根据重要级别决定先后执行顺序
};

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_QUEUE_ATOMIC_PRIORITY_QUEUE_H_
