/*
* Created by boil on 2023/10/26.
*/

#include "pipe_wrapper.h"
#include "socket/socket_util.h"
#include "utils/string_util.h"
#include "poller/errno/errno.h"

#define checkFD(fd) \
    if (fd == -1) { \
        clearFD(); \
        throw runtime_error(StrPrinter << "Create windows pipe failed: " << get_uv_errmsg());\
    }

#define closeFD(fd) \
    if (fd != -1) { \
        close(fd);\
        fd = -1;\
    }

RD_NAMESPACE_BEGIN

  PipeWrapper::PipeWrapper() {
    reOpen();
  }

  PipeWrapper::~PipeWrapper() {
    clearFD();
  }

  int PipeWrapper::Write(const void *buf, int n) {
    int ret;
    do {
#if defined(_WIN32)
      ret = send(_pipe_fd[1], (char *)buf, n, 0);
#else
      ret = ::write(_pipe_fd[1], buf, n);
#endif // defined(_WIN32)
    } while (-1 == ret && RD_EINTR == get_rd_error(true));
    return ret;
  }

  int PipeWrapper::Read(void *buf, int n) {
    int ret;
    do {
#if defined(_WIN32)
      ret = recv(_pipe_fd[0], (char *)buf, n, 0);
#else
      ret = ::read(_pipe_fd[0], buf, n);
#endif // defined(_WIN32)
    } while (-1 == ret && RD_EINTR == get_rd_error(true));
    return ret;
  }

  void PipeWrapper::reOpen() {
    clearFD();
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
    if (pipe(_pipe_fd) == -1) {
      throw std::runtime_error(StrPrinter << "Create posix pipe failed: " << get_rd_errmsg());
    }
#endif // defined(_WIN32)
    SockUtil::SetNoBlocked(_pipe_fd[0], true);
    SockUtil::SetNoBlocked(_pipe_fd[1], false);
    SockUtil::SetCloExec(_pipe_fd[0]);
    SockUtil::SetCloExec(_pipe_fd[1]);
  }


  void PipeWrapper::clearFD() {
    closeFD(_pipe_fd[0]);
    closeFD(_pipe_fd[1]);
  }
RD_NAMESPACE_END