/*
* Created by boil on 2023/9/27.
*/

#ifndef RENDU_BASE_CONCURRENT_QUEUE_H
#define RENDU_BASE_CONCURRENT_QUEUE_H

#include "base_define.h"
#include <concurrentqueue/concurrentqueue.h>

RD_NAMESPACE_BEGIN
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

RD_NAMESPACE_END

#endif //RENDU_BASE_CONCURRENT_QUEUE_H
