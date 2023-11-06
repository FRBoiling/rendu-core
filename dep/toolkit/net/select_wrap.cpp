/*
* Created by boil on 2023/10/26.
*/

#include <locale>
#include <sys/select.h>
#include "select_wrap.h"

RD_NAMESPACE_BEGIN

  FdSet::FdSet() {
    _ptr = new fd_set;
  }

  FdSet::~FdSet() {
    delete (fd_set *) _ptr;
  }

  void FdSet::fdZero() {
    FD_ZERO((fd_set *) _ptr);
  }

  void FdSet::fdClr(int fd) {
    FD_CLR(fd, (fd_set *) _ptr);
  }

  void FdSet::fdSet(int fd) {
    FD_SET(fd, (fd_set *) _ptr);
  }

  bool FdSet::isSet(int fd) {
    return FD_ISSET(fd, (fd_set *) _ptr);
  }

  int rd_select(int cnt, FdSet *read, FdSet *write, FdSet *err, struct timeval *tv) {
    void *rd, *wt, *er;
    rd = read ? read->_ptr : nullptr;
    wt = write ? write->_ptr : nullptr;
    er = err ? err->_ptr : nullptr;
    return ::select(cnt, (fd_set *) rd, (fd_set *) wt, (fd_set *) er, tv);
  }

RD_NAMESPACE_END