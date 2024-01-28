/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_SOCKET_EXCEPTION_H
#define RENDU_SOCKET_EXCEPTION_H

#include <sys/socket.h>
#include "define.h"
#include "utils/rd_errno.h"

RD_NAMESPACE_BEGIN

//错误类型枚举
  enum ErrCode {
    Err_success = 0, //成功
    Err_eof, //eof
    Err_timeout, //超时
    Err_refused,//连接被拒绝
    Err_dns,//dns解析失败
    Err_shutdown,//主动关闭
    Err_other = 0xFF,//其他错误
  };



//错误信息类
  class SockException : public std::exception {
  public:
    SockException(ErrCode code = Err_success, const std::string &msg = "", int custom_code = 0) {
      _msg = msg;
      _code = code;
      _custom_code = custom_code;
    }

    //错误提示
    const char *what() const noexcept override {
      return _msg.c_str();
    }

    //重置错误
    void Reset(ErrCode code, const std::string &msg, int custom_code = 0) {
      _msg = msg;
      _code = code;
      _custom_code = custom_code;
    }

    //错误代码
    ErrCode GetErrCode() const {
      return _code;
    }

    //用户自定义错误代码
    int GetCustomCode() const {
      return _custom_code;
    }

    //判断是否真的有错
    operator bool() const {
      return _code != Err_success;
    }

  private:
    ErrCode _code;
    int _custom_code = 0;
    std::string _msg;
  };

  static SockException ToSockException(int error) {
    switch (error) {
      case 0:
      case RD_EAGAIN: return SockException(Err_success, "success");
      case RD_ECONNREFUSED: return SockException(Err_refused, strerror(error), error);
      case RD_ETIMEDOUT: return SockException(Err_timeout, strerror(error), error);
      default: return SockException(Err_other, strerror(error), error);
    }
  }

  static SockException GetSockErr(int sock, bool try_errno = true) {
    int error = 0, len = sizeof(int);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&error, (socklen_t *)&len);
    if (error == 0) {
      if (try_errno) {
        error = GetError(true);
      }
    } else {
      error = TranslatePosixError(error);
    }
    return ToSockException(error);
  }

RD_NAMESPACE_END

#endif //RENDU_SOCKET_EXCEPTION_H
