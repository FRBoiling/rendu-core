/*
* Created by boil on 2023/9/27.
*/

#ifndef RENDU_COMMON_CONCURRENT_QUEUE_H
#define RENDU_COMMON_CONCURRENT_QUEUE_H

<<<<<<<< HEAD:rendu/common/common/concurrent/concurrent_queue.h
#include "common/define.h"
========
#include "common/common_define.h"
>>>>>>>> f6e2ef7 (✨ feat(框架): 整理项目结构):rendu/common/common/container/concurrent/concurrent_queue.h
#include <concurrentqueue/concurrentqueue.h>

COMMON_NAMESPACE_BEGIN
    template<typename T>
    class ConcurrentQueue{
    public:

      void push(const T &t) {
        moodycamel::ConcurrentQueue<T>::enqueue(t);
      }

      void push(T &&t) {
        moodycamel::ConcurrentQueue<T>::enqueue(std::move(t));
      }

      bool pop(T &t) {
        return moodycamel::ConcurrentQueue<T>::try_dequeue(t);
      }

      int Size() {
        return 0;
      }

      bool TryDequeue(T& t) {

        return true;
      }

      void Enqueue(T& t) {

      }

      void Enqueue(T&& t) {

      }
    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_CONCURRENT_QUEUE_H
