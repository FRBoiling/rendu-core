/*
* Created by boil on 2023/3/19.
*/

#include "hook.h"

template<typename Ret, typename... Args>
void rendu::hook<Ret(Args...)>::connect(std::function<Ret(Args...)> callback, bool front) {
  if (front) {
    m_callbacks.insert(m_callbacks.begin(), callback);
  } else {
    m_callbacks.push_back(callback);
  }

}

template<typename Ret, typename... Args>
void rendu::hook<Ret(Args...)>::publish(Args... args) {
  for (auto callback : m_callbacks) {
    callback(args...);
  }
  
}