/*
* Created by boil on 2023/10/30.
*/

#ifndef RENDU_TCP_CLIENT_H
#define RENDU_TCP_CLIENT_H

#include "a_service.h"
#include "socket/socket.h"
#include "session/session.h"
#include "session/session_helper.h"
#include "timer/timer.h"
#include "logger/logger.h"
#include "utils/instance_imp.h"

RD_NAMESPACE_BEGIN

//Tcp客户端，Socket对象默认开始互斥锁
  class TcpClient : public AService {
  public:
    using Ptr = std::shared_ptr<TcpClient>;

    TcpClient(const EventPoller::Ptr &poller = nullptr);

    ~TcpClient();

    /**
     * 开始连接tcp服务器
     * @param url 服务器ip或域名
     * @param port 服务器端口
     * @param timeout_sec 超时时间,单位秒
     * @param local_port 本地端口
     */
    template<typename SessionType>
    void Connect(const std::string &url, uint16_t port, float timeout_sec = 5, uint16_t local_port = 0) {
      static std::string cls_name = demangle(typeid(SessionType).name());
      //Session创建器，通过它创建不同类型的服务器
      _session_alloc = [](const TcpClient::Ptr &client, const Socket::Ptr &sock) {
        auto session = std::shared_ptr<SessionType>(new SessionType(sock), [](SessionType *ptr) {
          TraceP(static_cast<Session *>(ptr)) << "~" << cls_name;
          delete ptr;
        });
        TraceP(static_cast<Session *>(session.get())) << cls_name;
        session->setOnCreateSocket(client->_on_create_socket);
        return std::make_shared<SessionHelper>(client, std::move(session), cls_name);
      };
      connect(url, port, timeout_sec, local_port);
    }

    void connect(const std::string &url, uint16_t port, float timeout_sec = 5, uint16_t local_port = 0);

    void onManagerSession();

    /**
 * @brief 自定义socket构建行为
 */
    void setOnCreateSocket(Socket::onCreateSocket cb);
    /**
     * 根据socket对象创建Session对象
     * 需要确保在socket归属poller线程执行本函数
     */
    Session::Ptr createSession(const Socket::Ptr &socket);
    /**
     * 通过代理开始连接tcp服务器
     * @param url 服务器ip或域名
     * @proxy_host 代理ip
     * @proxy_port 代理端口
     * @param timeout_sec 超时时间,单位秒
     * @param local_port 本地端口
     */
    virtual void startConnectWithProxy(const std::string &url, const std::string &proxy_host, uint16_t proxy_port,
                                       float timeout_sec = 5, uint16_t local_port = 0) {};

    /**
     * 主动断开连接
     * @param ex 触发onErr事件时的参数
     */
    void Shutdown(const SockException &ex = SockException(Err_shutdown, "self shutdown"));

    /**
     * 连接中或已连接返回true，断开连接时返回false
     */
    virtual bool alive() const;

    /**
     * 设置网卡适配器,使用该网卡与服务器通信
     * @param local_ip 本地网卡ip
     */
    virtual void setNetAdapter(const std::string &local_ip);

    virtual void onSockConnect(const SockException &ex);

  protected:
    Session::Ptr GetSession() { return _session; }

    Socket::Ptr GetSocket() { return _socket; }

  private:
    mutable std::string _id;
    std::string _net_adapter = "::";
    std::shared_ptr<Timer> _timer;
    bool _is_on_manager = false;
    Socket::Ptr _socket;
    Session::Ptr _session;
    //对象个数统计
    ObjectStatistic<TcpClient> _statistic;


    Socket::onCreateSocket _on_create_socket;
    std::function<SessionHelper::Ptr(const TcpClient::Ptr &client, const Socket::Ptr &)> _session_alloc;
  };

RD_NAMESPACE_END

#endif //RENDU_TCP_CLIENT_H
