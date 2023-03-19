/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_FRAMEWORK_HOOK_H_
#define RENDU_CORE_CORE_FRAMEWORK_HOOK_H_

#include <vector>
#include "plugin.h"

namespace rendu {

template<typename Type>
class hook;

template<typename Ret, typename ...Args>
class hook<Ret(Args...)> {
 private:
  using callback_type = std::function<Ret(Args...)>;
  std::vector<callback_type> m_callbacks;

 public:
  void connect(callback_type callback, bool front = false) {
    if (front) {
      m_callbacks.insert(m_callbacks.begin(), callback);
    } else {
      m_callbacks.push_back(callback);
    }
  }
  void publish(Args... args) {
    for (auto callback : m_callbacks) {
      callback(args...);
    }
  }
};

}

#endif //RENDU_CORE_CORE_FRAMEWORK_HOOK_H_
