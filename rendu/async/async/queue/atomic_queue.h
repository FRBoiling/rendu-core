/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_QUEUE_ATOMIC_QUEUE_H_
#define RENDU_ASYNC_ASYNC_QUEUE_ATOMIC_QUEUE_H_

#include "queue_object.h"
#include "thread_pool/thread_pool_define.h"
#include "lock/lock_include.h"

RD_NAMESPACE_BEGIN

template<typename T>
class AtomicQueue : public QueueObject{
public:
  AtomicQueue() = default;

  /**
     * 等待弹出
     * @param value
     */
  Void waitPop(T& value) {
    RD_UNIQUE_LOCK lk(mutex_);
    cv_.wait(lk, [this] { return !queue_.empty(); });
    value = std::move(*queue_.front());
    queue_.pop();
  }


  /**
     * 尝试弹出
     * @param value
     * @return
     */
  Bool tryPop(T& value) {
    Bool result = false;
    if (!queue_.empty() && mutex_.try_lock()) {
      if (!queue_.empty()) {
        value = std::move(*queue_.front());
        queue_.pop();
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
    if (!queue_.empty() && mutex_.try_lock()) {
      while (!queue_.empty() && maxPoolBatchSize-- > 0) {
        values.emplace_back(std::move(*queue_.front()));
        queue_.pop();
        result = true;
      }
      mutex_.unlock();
    }

    return result;
  }


  /**
     * 阻塞式等待弹出
     * @return
     */
  std::unique_ptr<T> popWithTimeout(MSec ms) {
    RD_UNIQUE_LOCK lk(mutex_);
    if (!cv_.wait_for(lk, std::chrono::milliseconds(ms), [this] { return !queue_.empty(); })) {
      return nullptr;
    }

    std::unique_ptr<T> result = std::move(queue_.front());
    queue_.pop();    // 如果等成功了，则弹出一个信息
    return result;
  }


  /**
     * 非阻塞式等待弹出
     * @return
     */
  std::unique_ptr<T> tryPop() {
    RD_LOCK_GUARD lk(mutex_);
    if (queue_.empty()) { return std::unique_ptr<T>(); }
    std::unique_ptr<T> ptr = std::move(queue_.front());
    queue_.pop();
    return ptr;
  }


  /**
     * 传入数据
     * @param value
     */
  Void push(T&& value) {
    std::unique_ptr<typename std::remove_reference<T>::type>     \
        task(make_unique<typename std::remove_reference<T>::type>(std::forward<T>(value)));
    while (true) {
      if (mutex_.try_lock()) {
        queue_.push(std::move(task));
        mutex_.unlock();
        break;
      } else {
        std::this_thread::yield();
      }
    }
    cv_.notify_one();
  }


  /**
     * 判定队列是否为空
     * @return
     */
  Bool empty() {
    RD_LOCK_GUARD lk(mutex_);
    return queue_.empty();
  }

  RD_NON_COPYABLE(AtomicQueue)

private:
  std::queue<std::unique_ptr<T>> queue_;
};

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_QUEUE_ATOMIC_QUEUE_H_
