/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_BASE_NON_COPYABLE_H
#define RENDU_BASE_NON_COPYABLE_H

#include "basic_define.h"

RD_NAMESPACE_BEGIN



class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;

  void operator=(const NonCopyable &) = delete;

protected:
  NonCopyable() = default;

  ~NonCopyable() = default;
};

/** 定义为不能赋值和拷贝的对象类型 */
#define RD_NON_COPYABLE(Type)                                   \
    Type(const Type &) = delete;                                      \
    const Type &operator=(const Type &) = delete;

RD_NAMESPACE_END

#endif//RENDU_BASE_NON_COPYABLE_H
