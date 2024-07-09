/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_THREAD_THREAD_HPP
#define RENDU_THREAD_THREAD_HPP

#include "system_thread.h"
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <future>
#include "this_thread.h"

// 定义任务类型
class Thread {
  using Task = std::function<void()>;//定义类型
private:
  std::string m_name;
  std::mutex name_mutex;
  unsigned long m_id;
  std::thread m_thread;
  Task m_task;
  const std::string DEFAULT_THREAD_NAME = "RDThread";

public:
  Thread();

  template<class _Fp, class... _Args>
  explicit Thread(_Fp &&__f, _Args &&...__args) {
    SetThreadName(DEFAULT_THREAD_NAME);
    m_id = ThisThread::GetNextId();
//    using RetType = decltype(__f(__args...));// typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
    m_task = std::bind(std::forward<_Fp>(__f), std::forward<_Args>(__args)...);
  }

  ~Thread();

  // 确保类支持移动语义
  Thread(Thread &&other) noexcept
      : m_thread(std::move(other.m_thread)), m_name(std::move(other.m_name)), m_id(other.m_id) {
    other.m_id = 0;// 或者设置为无效值
  }

  Thread &operator=(Thread &&other) noexcept {
    if (this != &other) {
      if (m_thread.joinable()) {
        m_thread.join();// 确保在移动之前结束线程
      }
      m_thread = std::move(other.m_thread);
      m_name = std::move(other.m_name);
      m_id = other.m_id;
      other.m_id = 0;// 或者设置为无效值
    }
    return *this;
  }

public:
  // 获取当前线程的ID
  unsigned long GetId() {
    return m_id;
  }

  void SetThreadName(std::string name);
  std::string GetThreadName();

  void Start();

  bool IsAlive() const;

  void Join();

  void Detach();

  std::string ToString();
};


#endif//RENDU_THREAD_THREAD_HPP
