/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LIST_EXP_H
#define RENDU_LIST_EXP_H

#include <list>
#include <type_traits>

#include "define.h"

RD_NAMESPACE_BEGIN

  template<typename T>
  class List : public std::list<T> {
  public:
    template<typename ... ARGS>
    List(ARGS &&...args) : std::list<T>(std::forward<ARGS>(args)...) {};

    ~List() = default;

    void Append(List<T> &other) {
      if (other.empty()) {
        return;
      }
      this->insert(this->end(), other.begin(), other.end());
      other.clear();
    }

    template<typename FUNC>
    void For_Each(FUNC &&func) {
      for (auto &t: *this) {
        func(t);
      }
    }

    template<typename FUNC>
    void For_Each(FUNC &&func) const {
      for (auto &t: *this) {
        func(t);
      }
    }
  };

RD_NAMESPACE_END

#endif //RENDU_LIST_EXP_H
