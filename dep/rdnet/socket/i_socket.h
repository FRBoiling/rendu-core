/*
* Created by boil on 2023/10/22.
*/

#ifndef RENDU_I_SOCKET_H
#define RENDU_I_SOCKET_H


#include <cstdio>
#include "net_fwd.h"
#include "address_family.h"
#include "protocol_type.h"
#include "socket_type.h"
#include "endpoint/ip_end_point.h"

RD_NAMESPACE_BEGIN

  class ISocket {
  public:
    ISocket() : ISocket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::IPv4){};

    ISocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
      : m_af(addressFamily), m_st(socketType), m_pt(protocolType), m_sfd(-1) {};

    virtual ~ISocket() = default;

  public:
    AddressFamily m_af;
    SocketType m_st;
    ProtocolType m_pt;
    int m_sfd;
  public:
    virtual EndPoint *GetEndPoint() = 0;

    virtual void Bind(EndPoint *endPoint) = 0;

    virtual void Close() = 0;

    virtual void Listen(int backlog) = 0;

    virtual void Connect(const char *remote_host, uint16_t remote_port) = 0;

    virtual ISocket *Accept() = 0;

  public:
    //获取本机ip
    virtual std::string GetLocalIp() = 0;

    //获取本机端口号
    virtual uint16_t GetLocalPort() = 0;

    //获取远程ip
    virtual std::string GetRemoteIp() = 0;

    //获取远程端口号
    virtual uint16_t GetRemotePort() = 0;

    //获取标识符
    virtual std::string GetIdentifier() const { return ""; }
  };


#if defined(MSG_NOSIGNAL)
#define FLAG_NOSIGNAL MSG_NOSIGNAL
#else
#define FLAG_NOSIGNAL 0
#endif //MSG_NOSIGNAL

#if defined(MSG_MORE)
#define FLAG_MORE MSG_MORE
#else
#define FLAG_MORE 0
#endif //MSG_MORE

#if defined(MSG_DONTWAIT)
#define FLAG_DONTWAIT MSG_DONTWAIT
#else
#define FLAG_DONTWAIT 0
#endif //MSG_DONTWAIT

//默认的socket flags:不触发SIGPIPE,非阻塞发送
#define SOCKET_DEFAULE_FLAGS (FLAG_NOSIGNAL | FLAG_DONTWAIT )

//发送超时时间，如果在规定时间内一直没有发送数据成功，那么将触发onErr事件
#define SEND_TIME_OUT_SEC 10

RD_NAMESPACE_END

#endif //RENDU_I_SOCKET_H
