/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_COMMON_QUEUE_H
#define RENDU_COMMON_QUEUE_H

#include "common/common_define.h"
#include <queue>

COMMON_NAMESPACE_BEGIN

    template<typename Element>
    class Queue : public std::queue<Element> {

    public:
      int Count() {
        return std::queue<Element>::size();
      }

      [[nodiscard]] Element Dequeue() noexcept {
        Element element = std::move(this->front());
        this->pop();
        return element;
      }

      void Enqueue(Element &element) {
        this->push(element);
      }

      void Enqueue(Element &&element) {
        this->push(element);
      }

    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_QUEUE_H
