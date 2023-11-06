/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_TYPE_UTIL_H
#define RENDU_TYPE_UTIL_H

#include "common/define.h"

RD_NAMESPACE_BEGIN

  template<typename T>
  struct is_pointer : public std::false_type {
  };

  template<typename T>
  struct is_pointer<std::shared_ptr<T> > : public std::true_type {
  };

  template<typename T>
  struct is_pointer<std::shared_ptr<T const> > : public std::true_type {
  };

  template<typename T>
  struct is_pointer<T *> : public std::true_type {
  };

  template<typename T>
  struct is_pointer<const T *> : public std::true_type {
  };

RD_NAMESPACE_END

#endif //RENDU_TYPE_UTIL_H
