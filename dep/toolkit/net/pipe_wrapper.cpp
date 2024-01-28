/*
* Created by boil on 2023/10/26.
*/

#include "pipe_wrapper.h"
#include "utils/string_util.h"
#include "utils/rd_errno.h"
#include "utils/socket_helper.h"

RD_NAMESPACE_BEGIN

  PipeWrapper::PipeWrapper() {
    ReOpen();
  }

  PipeWrapper::~PipeWrapper() {
    ClearFD();
  }

  int PipeWrapper::Write(const void *buf, int n) {
    int ret;
    do {
#if defined(_WIN32)
      ret = send(_pipe_fd[1], (char *)buf, n, 0);
#else
      ret = SocketHelper::Write(_pipe_fd[1], buf, n);
#endif // defined(_WIN32)
    } while (-1 == ret && RD_EINTR == GetError(true));
    return ret;
  }

  int PipeWrapper::Read(void *buf, int n) {
    int ret;
    do {
#if defined(_WIN32)
      ret = recv(_pipe_fd[0], (char *)buf, n, 0);
#else
      ret = SocketHelper::Read(_pipe_fd[0], buf, n);
#endif // defined(_WIN32)
    } while (-1 == ret && RD_EINTR == GetError(true));
    return ret;
  }

  void PipeWrapper::ReOpen() {
    ClearFD();
#if defined(_WIN32)
    const char *localip = SockUtil::support_ipv6() ? "::1" : "127.0.0.1";
    auto listener_fd = SockUtil::listen(0, localip);
    checkFD(listener_fd)
    SockUtil::setNoBlocked(listener_fd,false);
    auto localPort = SockUtil::get_local_port(listener_fd);
    _pipe_fd[1] = SockUtil::connect(localip, localPort,false);
    checkFD(_pipe_fd[1])
    _pipe_fd[0] = (int)accept(listener_fd, nullptr, nullptr);
    checkFD(_pipe_fd[0])
    SockUtil::setNoDelay(_pipe_fd[0]);
    SockUtil::setNoDelay(_pipe_fd[1]);
    close(listener_fd);
#else
    if (SocketHelper::Pipe(_pipe_fd) == -1) {
      throw std::runtime_error(StrPrinter << "Create posix pipe failed: " << GetErrorMsg());
    }
#endif // defined(_WIN32)
    SocketHelper::SetNoBlocked(_pipe_fd[0], true);
    SocketHelper::SetNoBlocked(_pipe_fd[1], false);
    SocketHelper::SetCloExec(_pipe_fd[0]);
    SocketHelper::SetCloExec(_pipe_fd[1]);
  }


  void PipeWrapper::ClearFD() {
    SocketHelper::Close(_pipe_fd[0]);
    SocketHelper::Close(_pipe_fd[1]);
    _pipe_fd[0] = -1;
    _pipe_fd[1] = -1;
  }
RD_NAMESPACE_END