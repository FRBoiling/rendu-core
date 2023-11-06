/*
* Created by boil on 2023/11/10.
*/

#ifndef RENDU_INTERRUPT_EXCEPTION_H
#define RENDU_INTERRUPT_EXCEPTION_H

#include "define.h"
#include "utils/rd_errno.h"

RD_NAMESPACE_BEGIN

//中断错误
  class InterruptException : public std::runtime_error {
  public:
    InterruptException() : std::runtime_error("InterruptException") {}

    ~InterruptException() {}
  };

RD_NAMESPACE_END

#endif //RENDU_INTERRUPT_EXCEPTION_H
