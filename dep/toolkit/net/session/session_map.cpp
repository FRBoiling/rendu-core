/*
* Created by boil on 2023/10/26.
*/

#include "session_map.h"

RD_NAMESPACE_BEGIN

  INSTANCE_IMP(SessionMap)

  bool SessionMap::add(const string &tag, const Session::Ptr &session) {
    std::lock_guard<std::mutex> lck(_mtx_session);
    return _map_session.emplace(tag, session).second;
  }

  bool SessionMap::del(const string &tag) {
    std::lock_guard<std::mutex> lck(_mtx_session);
    return _map_session.erase(tag);
  }

  Session::Ptr SessionMap::get(const string &tag) {
    std::lock_guard<std::mutex> lck(_mtx_session);
    auto it = _map_session.find(tag);
    if (it == _map_session.end()) {
      return nullptr;
    }
    return it->second.lock();
  }

  void SessionMap::for_each_session(const std::function<void(const string &id, const Session::Ptr &session)> &cb) {
    std::lock_guard<std::mutex> lck(_mtx_session);
    for (auto it = _map_session.begin(); it != _map_session.end();) {
      auto session = it->second.lock();
      if (!session) {
        it = _map_session.erase(it);
        continue;
      }
      cb(it->first, session);
      ++it;
    }
  }



RD_NAMESPACE_END