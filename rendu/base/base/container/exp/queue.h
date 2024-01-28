/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_CONTAINER_EXP_QUEUE_H_
#define RENDU_BASE_CONTAINER_EXP_QUEUE_H_

#include "base_define.h"
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

#endif//RENDU_BASE_CONTAINER_EXP_QUEUE_H_
