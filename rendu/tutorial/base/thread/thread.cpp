/*
* Created by boil on 2024/1/29.
*/

#include "thread.h"

#include <utility>



Thread::Thread() {
}

Thread::~Thread() {
  if (IsAlive()) {
    m_thread.join();// 确保在析构时线程被正确回收
  }
}


void  Thread::SetThreadName(std::string name) {
  std::lock_guard<std::mutex> lock(name_mutex);
  m_name = std::move(name);
}

std::string  Thread::GetThreadName() {
  std::lock_guard<std::mutex> lock(name_mutex);
  return m_name;
}

std::string Thread::ToString() {
  return std::format("{}-{}", m_name, m_id);
}


bool Thread::IsAlive() const {
  return m_thread.joinable();
}

void Thread::Join() {
  if (IsAlive()) {
    m_thread.join();
  }
}

void Thread::Detach() {
  if (IsAlive()) {
    m_thread.detach();
  }
}

void  Thread::Start(){
  auto func = ([this]() {
    ThisThread::SetCurrentThreadId(this->m_id);
    try {
      m_task();
    } catch (const std::exception &e) {
      std::cerr << "Exception in thread " << this->m_id << ": " << e.what() << std::endl;
    }
  });
  m_thread = std::thread(func);
}
