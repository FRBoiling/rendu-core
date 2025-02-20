/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_STD_EX_H_
#define RENDU_BASIC_BASIC_BASE_STD_EX_H_

#include "basic_define.h"
#include <memory>
#include <type_traits>

RD_NAMESPACE_BEGIN

// 兼容 std::enable_if_t 的语法
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

// 兼容 std::make_unique 的语法
template<typename T, typename... Args>
typename std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_STD_EX_H_
