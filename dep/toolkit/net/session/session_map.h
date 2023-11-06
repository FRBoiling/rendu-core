/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_SESSION_MAP_H
#define RENDU_SESSION_MAP_H


#include "session.h"
#include "utils/instance_imp.h"

RD_NAMESPACE_BEGIN

// 全局的 Session 记录对象, 方便后面管理
// 线程安全的
  class SessionMap : public std::enable_shared_from_this<SessionMap> {
  public:
    friend class SessionHelper;

    using Ptr = std::shared_ptr<SessionMap>;

    //单例
    static SessionMap &Instance();

    ~SessionMap() = default;

    //获取Session
    Session::Ptr get(const std::string &tag);

    void for_each_session(const std::function<void(const std::string &id, const Session::Ptr &session)> &cb);

  private:
    SessionMap() = default;

    //移除Session
    bool del(const std::string &tag);

    //添加Session
    bool add(const std::string &tag, const Session::Ptr &session);

  private:
    std::mutex _mtx_session;
    std::unordered_map<std::string, std::weak_ptr<Session> > _map_session;
  };


RD_NAMESPACE_END

#endif //RENDU_SESSION_MAP_H
