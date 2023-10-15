/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_SELECT_WRAP_H
#define RENDU_SELECT_WRAP_H

#include "define.h"

RD_NAMESPACE_BEGIN

  class FdSet {
  public:
    FdSet();

    ~FdSet();

    void fdZero();

    void fdSet(int fd);

    void fdClr(int fd);

    bool isSet(int fd);

    void *_ptr;
  };

  int rd_select(int cnt, FdSet *read, FdSet *write, FdSet *err, struct timeval *tv);

RD_NAMESPACE_END

#endif //RENDU_SELECT_WRAP_H
