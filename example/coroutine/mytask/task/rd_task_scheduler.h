/*
* Created by boil on 2023/12/17.
*/

#ifndef RENDU_RD_TASK_SCHEDULER_H
#define RENDU_RD_TASK_SCHEDULER_H

#include <functional>
#include <future>

class RDTaskScheduler {
public:
  static void Dispatch(std::function<void()> &&func,bool async) {
    if (async){
      auto future = std::async(func);
    }
    else  {
      func();
    }
  }
};


#endif//RENDU_RD_TASK_SCHEDULER_H
