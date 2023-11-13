/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_BASE_BASE_CONTAINER_EXP_LIST_H_
#define RENDU_BASE_BASE_CONTAINER_EXP_LIST_H_


#include "base_define.h"
#include <list>

RD_NAMESPACE_BEGIN

template<typename T>
class CList : public std::list<T> {
public:
  template<typename... ARGS>
  CList(ARGS &&...args) : std::list<T>(std::forward<ARGS>(args)...){};

  ~CList() = default;

  void Append(CList<T> &other) {
    if (other.empty()) {
      return;
    }
    this->insert(this->end(), other.begin(), other.end());
    other.clear();
  }

  template<typename FUNC>
  void ForEach(FUNC &&func) {
    for (auto &t: *this) {
      func(t);
    }
  }

  template<typename FUNC>
  void ForEach(FUNC &&func) const {
    for (auto &t: *this) {
      func(t);
    }
  }
};

RD_NAMESPACE_END

#endif//RENDU_BASE_BASE_CONTAINER_EXP_LIST_H_
