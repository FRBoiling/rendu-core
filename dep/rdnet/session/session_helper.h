/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_SESSION_HELPER_H
#define RENDU_SESSION_HELPER_H

#include "session.h"
#include "session_map.h"

RD_NAMESPACE_BEGIN

  class AService;

  class SessionHelper {
  public:
    using Ptr = std::shared_ptr<SessionHelper>;

    SessionHelper(const std::weak_ptr<AService> &server, Session::Ptr session, std::string cls);

    ~SessionHelper();

    const Session::Ptr &session() const;

    const std::string &className() const;

  private:
    std::string _cls;
    std::string _identifier;
    Session::Ptr _session;
    SessionMap::Ptr _session_map;
    std::weak_ptr<AService> _server;
  };

RD_NAMESPACE_END

#endif //RENDU_SESSION_HELPER_H
