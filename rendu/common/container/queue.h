/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_QUEUE_H
#define RENDU_QUEUE_H

#include "define.h"
#include <queue>

RD_NAMESPACE_BEGIN

    template<typename T>
    class Queue : public std::queue<T> {

    public:
      T Dequeue() {
        T element = this->front();
        this->pop();
        return element;
      }

    };

RD_NAMESPACE_END

#endif //RENDU_QUEUE_H
