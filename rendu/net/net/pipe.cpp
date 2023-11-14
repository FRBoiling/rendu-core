/*
* Created by boil on 2023/11/8.
*/

#include "pipe.h"
#include "sockets/sock_ops.h"
#include "errno/errno.h"

COMMON_NAMESPACE_BEGIN

  Pipe::Pipe() {
    ReOpen();
  }

  Pipe::~Pipe() {
    ClearFD();
  }

  void Pipe::ReOpen() {
    ClearFD();
    if (SockOps::Pipe(_pipe_fd) == -1) {
//      throw runtime_error(StrPrinter << "Create posix pipe failed: " << get_uv_errmsg());
    }
    SockOps::SetNoBlocked(_pipe_fd[0], true);
    SockOps::SetNoBlocked(_pipe_fd[1], false);
    SockOps::SetCloExec(_pipe_fd[0]);
    SockOps::SetCloExec(_pipe_fd[1]);
  }

  void Pipe::ClearFD() {
    SockOps::Close(_pipe_fd[0]);
    SockOps::Close(_pipe_fd[1]);
  }


  int Pipe::Write(const void *buf, int n) {
    int ret;
    do {
      ret = (int)SockOps::Write(_pipe_fd[1], buf, n);
    } while (-1 == ret && EINTR == GetError(true));
    return ret;
  }

  int Pipe::Read(void *buf, int n) {
    int ret;
    do {
      ret = (int)SockOps::Read(_pipe_fd[0], buf, n);
    } while (-1 == ret && EINTR == GetError(true));
    return ret;
  }

COMMON_NAMESPACE_END