/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_MEMORY_COPY_H
#define RENDU_BASE_MEMORY_COPY_H


#include "number/int_helper.h"

RD_NAMESPACE_BEGIN

  template<typename T>
  void Copy(const T *src, INT32 src_index, T *dest, INT32 dest_index, INT32 length);

  template<typename T>
  void Copy(const T *src, T *dest);

RD_NAMESPACE_END

#endif //RENDU_BASE_MEMORY_COPY_H
