/*
* Created by boil on 2023/10/26.
*/

#include "thread_group.h"

RD_NAMESPACE_BEGIN

  ThreadGroup::~ThreadGroup() {
    _threads.clear();
  }


  bool ThreadGroup::IsThisThreadIn() {
    auto thread_id = std::this_thread::get_id();
    if (_thread_id == thread_id) {
      return true;
    }
    return _threads.find(thread_id) != _threads.end();
  }

  bool ThreadGroup::IsThreadIn(std::thread *thread) {
    if (!thread) {
      return false;
    }
    auto it = _threads.find(thread->get_id());
    return it != _threads.end();
  }

  void ThreadGroup::RemoveThread(std::thread *thread) {
    auto it = _threads.find(thread->get_id());
    if (it != _threads.end()) {
      _threads.erase(it);
    }
  }

  void ThreadGroup::JoinAll() {
    if (IsThisThreadIn()) {
      throw std::runtime_error("Trying joining itself in ThreadGroup");
    }
    for (auto &it: _threads) {
      if (it.second->joinable()) {
        it.second->join(); //等待线程主动退出
      }
    }
    _threads.clear();
  }

  size_t ThreadGroup::Size() {
    return _threads.size();
  }


RD_NAMESPACE_END