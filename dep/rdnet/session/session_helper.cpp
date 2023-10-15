/*
* Created by boil on 2023/10/26.
*/

#include "session_helper.h"

RD_NAMESPACE_BEGIN

  SessionHelper::SessionHelper(const std::weak_ptr<AService> &server, Session::Ptr session, std::string cls) {
    _server = server;
    _session = std::move(session);
    _cls = std::move(cls);
    //记录session至全局的map，方便后面管理
    _session_map = SessionMap::Instance().shared_from_this();
    _identifier = _session->GetIdentifier();
    _session_map->add(_identifier, _session);
  }

  SessionHelper::~SessionHelper() {
    if (!_server.lock()) {
      //务必通知Session已从TcpServer脱离
      _session->onError(SockException(Err_other, "Server shutdown"));
    }
    //从全局map移除相关记录
    _session_map->del(_identifier);
  }

  const Session::Ptr &SessionHelper::session() const {
    return _session;
  }

  const std::string &SessionHelper::className() const {
    return _cls;
  }


RD_NAMESPACE_END