/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_QUEUE_H
#define RENDU_QUEUE_H

#include "define.h"
#include <queue>

RD_NAMESPACE_BEGIN

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

RD_NAMESPACE_END

#endif //RENDU_QUEUE_H
