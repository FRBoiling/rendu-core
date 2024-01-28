/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_DETAIL_REMOVE_RVALUE_REFERENCE_HPP
#define RENDU_ASYNC_DETAIL_REMOVE_RVALUE_REFERENCE_HPP

ASYNC_NAMESPACE_BEGIN

namespace detail {
  template<typename T>
  struct remove_rvalue_reference {
    using type = T;
  };

  template<typename T>
  struct remove_rvalue_reference<T &&> {
    using type = T;
  };

  template<typename T>
  using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;
}// namespace detail

ASYNC_NAMESPACE_END

#endif
