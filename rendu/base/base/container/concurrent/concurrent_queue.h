/*
* Created by boil on 2023/9/27.
*/

#ifndef RENDU_BASE_CONCURRENT_QUEUE_H
#define RENDU_BASE_CONCURRENT_QUEUE_H

#include "base_define.h"
#include "boost/lockfree/queue.hpp"


RD_NAMESPACE_BEGIN
template<typename T>
class ConcurrentQueue {
private:
  boost::lockfree::queue<T> queue_;

public:
  ConcurrentQueue() : queue_(128) {}// 初始化队列大小

  void push(const T &value) {
    queue_.push(value);
  }

  bool try_pop(T &popped_value) {
    return queue_.pop(popped_value);
  }

  bool empty() const {
    return queue_.empty();
  }
};

RD_NAMESPACE_END

#endif//RENDU_BASE_CONCURRENT_QUEUE_H
