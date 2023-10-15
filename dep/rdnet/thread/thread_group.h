/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_THREAD_GROUP_H
#define RENDU_THREAD_GROUP_H

#include <stdexcept>
#include <thread>
#include <unordered_map>
#include "define.h"

RD_NAMESPACE_BEGIN

  class ThreadGroup {

  public:
    ThreadGroup() = default;

    ~ThreadGroup();

  private:
    ThreadGroup(ThreadGroup const &);

    ThreadGroup &operator=(ThreadGroup const &);
  public:
    template<typename Func>
    std::thread *CreateThread(Func &&thread_func) {
      auto thread_new = std::make_shared<std::thread>(thread_func);
      _thread_id = thread_new->get_id();
      _threads[_thread_id] = thread_new;
      return thread_new.get();
    }

    void RemoveThread(std::thread *thread);


    bool IsThisThreadIn();

    bool IsThreadIn(std::thread *thread);

    void JoinAll();

    size_t Size();

  private:
    std::thread::id _thread_id;
    std::unordered_map<std::thread::id, std::shared_ptr<std::thread>> _threads;
  };

RD_NAMESPACE_END

#endif //RENDU_THREAD_GROUP_H
