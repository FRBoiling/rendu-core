/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_FRAMEWORK_HOOK_H_
#define RENDU_CORE_CORE_FRAMEWORK_HOOK_H_

#include <vector>

namespace rendu {

template<typename Type>
class hook;

template<typename Ret, typename ...Args>
class hook<Ret(Args...)> {
 private:
  using callback_type = std::function<Ret(Args...)>;
  std::vector<callback_type> m_callbacks;

 public:
  void connect(callback_type callback, bool front = false);
  void publish(Args... args);
};

}

#endif //RENDU_CORE_CORE_FRAMEWORK_HOOK_H_
