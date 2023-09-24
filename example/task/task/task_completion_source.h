/*
* Created by boil on 2023/10/9.
*/

#ifndef RENDU_TASK_COMPLETION_SOURCE_H
#define RENDU_TASK_COMPLETION_SOURCE_H

#include "define.h"
#include "task.h"

RD_NAMESPACE_BEGIN
    template<typename T>
    class TaskCompletionSource {
    public:
      TaskCompletionSource(){
        _task = new Task<T>();
      }

    private:
      Task<T>* _task;
    public:
      Task<T>* GetTask() {
        co_return *_task;
      }
      void SetResult(T result){}

    };
RD_NAMESPACE_END


#endif //RENDU_TASK_COMPLETION_SOURCE_H
