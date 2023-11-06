/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_SOCKET_FD_H
#define RENDU_SOCKET_FD_H

#include "socket_num.h"

RD_NAMESPACE_BEGIN

//socket 文件描述符的包装
//在析构时自动溢出监听并close套接字
//防止描述符溢出
  class SocketFD {
  public:
    using Ptr = std::shared_ptr<SocketFD>;

    /**
     * 创建一个fd对象
     * @param num 文件描述符，int数字
     */
    SocketFD(int num, SocketNum::SockType type);

    /**
     * 复制一个fd对象
     * @param that 源对象
     */
    SocketFD(const SocketFD &that);

    ~SocketFD();

  public:

    SocketNum::SockType GetType() const;

    [[nodiscard]] int32 RawFd() const;


    void DelEvent();

    void SetConnected();

  private:
    SocketNum::Ptr _num;
  };

RD_NAMESPACE_END

#endif //RENDU_SOCKET_FD_H
