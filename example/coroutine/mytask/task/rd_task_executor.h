/*
* Created by boil on 2023/12/17.
*/

#ifndef RENDU_RD_TASK_EXECUTOR_H
#define RENDU_RD_TASK_EXECUTOR_H

#include <functional>

class RDTaskExecutor {
public:
  virtual void Execute(std::function<void()> &&func){

  };
};


#endif//RENDU_RD_TASK_EXECUTOR_H
