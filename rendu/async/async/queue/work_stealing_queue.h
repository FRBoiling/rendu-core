/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_QUEUE_WORK_STEALING_QUEUE_H_
#define RENDU_ASYNC_ASYNC_QUEUE_WORK_STEALING_QUEUE_H_

#include <deque>
#include "queue_object.h"

RD_NAMESPACE_BEGIN

template<typename T>
class WorkStealingQueue : public QueueObject {
public:
  /**
     * 向队列中写入信息
     * @param task
     */
  Void push(T&& task) {
    while (true) {
      if (lock_.try_lock()) {
        deque_.emplace_front(std::forward<T>(task));
        lock_.unlock();
        break;
      } else {
        sched_yield();
      }
    }
  }
  

  /**
     * 尝试往队列里写入信息
     * @param task
     * @return
     */
  Bool tryPush(T&& task) {
    Bool result = false;
    if (lock_.try_lock()) {
      deque_.emplace_back(std::forward<T>(task));
      lock_.unlock();
      result = true;
    }
    return result;
  }


  /**
     * 向队列中写入信息
     * @param task
     */
  Void push(std::vector<T>& tasks) {
    while (true) {
      if (lock_.try_lock()) {
        for (const auto& task : tasks) {
          deque_.emplace_front(std::forward<T>(task));
        }
        lock_.unlock();
        break;
      } else {
        sched_yield();
      }
    }
  }


  /**
     * 尝试批量写入内容
     * @param tasks
     * @return
     */
  Bool tryPush(std::vector<T>& tasks) {
    Bool result = false;
    if (lock_.try_lock()) {
      for (const auto& task : tasks) {
        deque_.emplace_back(std::forward<T>(task));
      }
      lock_.unlock();
      result = true;
    }
    return result;
  }


  /**
     * 弹出节点，从头部进行
     * @param task
     * @return
     */
  Bool tryPop(T& task) {
    // 这里不使用raii锁，主要是考虑到多线程的情况下，可能会重复进入
    bool result = false;
    if (!deque_.empty() && lock_.try_lock()) {
      if (!deque_.empty()) {
        task = std::forward<T>(deque_.front());    // 从前方弹出
        deque_.pop_front();
        result = true;
      }
      lock_.unlock();
    }

    return result;
  }


  /**
     * 从头部开始批量获取可执行任务信息
     * @param taskArr
     * @param maxLocalBatchSize
     * @return
     */
  Bool tryPop(std::vector<T>& taskArr, int maxLocalBatchSize) {
    bool result = false;
    if (!deque_.empty() && lock_.try_lock()) {
      while (!deque_.empty() && maxLocalBatchSize--) {
        taskArr.emplace_back(std::forward<T>(deque_.front()));
        deque_.pop_front();
        result = true;
      }
      lock_.unlock();
    }

    return result;
  }


  /**
     * 窃取节点，从尾部进行
     * @param task
     * @return
     */
  Bool trySteal(T& task) {
    bool result = false;
    if (!deque_.empty() && lock_.try_lock()) {
      if (!deque_.empty()) {
        task = std::forward<T>(deque_.back());    // 从后方窃取
        deque_.pop_back();
        result = true;
      }
      lock_.unlock();
    }

    return result;
  }


  /**
     * 批量窃取节点，从尾部进行
     * @param taskArr
     * @return
     */
  Bool trySteal(std::vector<T>& taskArr, int maxStealBatchSize) {
    bool result = false;
    if (!deque_.empty() && lock_.try_lock()) {
      while (!deque_.empty() && maxStealBatchSize--) {
        taskArr.emplace_back(std::forward<T>(deque_.back()));
        deque_.pop_back();
        result = true;
      }
      lock_.unlock();
    }

    return result;    // 如果非空，表示盗取成功
  }

  WorkStealingQueue() = default;

  RD_NON_COPYABLE(WorkStealingQueue)

private:
  std::deque<T> deque_;            // 存放任务的双向队列
  std::mutex lock_;                // 用于处理deque_的锁
};

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_QUEUE_WORK_STEALING_QUEUE_H_
