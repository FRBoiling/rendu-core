/*
* Created by boil on 2023/12/27.
*/

#ifndef RENDU_COROUTINE_DEFINE_H
#define RENDU_COROUTINE_DEFINE_H


#include <stdexcept>

inline void logicAssert(bool x, const char* errorMsg) {
  if (x)
     return;
  throw std::logic_error(errorMsg);
}


#endif//RENDU_COROUTINE_DEFINE_H
