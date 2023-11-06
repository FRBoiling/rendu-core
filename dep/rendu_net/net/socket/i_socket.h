/*
* Created by boil on 2023/10/22.
*/

#ifndef RENDU_I_SOCKET_H
#define RENDU_I_SOCKET_H


#include <cstdio>
#include "socket_type.h"
#include "endpoint/ip_end_point.h"
#include "address/address_family.h"
#include "protocol_type.h"

RD_NAMESPACE_BEGIN

  class ISocket {
  public:
    ISocket() : ISocket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::IPv4) {};

    ISocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
      : m_af(addressFamily), m_st(socketType), m_pt(protocolType), m_sfd(-1) {};

    virtual ~ISocket() = default;

  public:
    virtual IPEndPoint *GetEndPoint() = 0;

    virtual void Bind(IPEndPoint *endPoint) = 0;

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

  public:
    int32 GetFD() { return m_sfd; }
  private:
    AddressFamily m_af;
    SocketType m_st;
    ProtocolType m_pt;
    int32 m_sfd;
  };


RD_NAMESPACE_END

#endif //RENDU_I_SOCKET_H
