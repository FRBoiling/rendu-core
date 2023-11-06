/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_TASK_CANCELABLE_H
#define RENDU_TASK_CANCELABLE_H

#include "i_task_cancelable.h"

RD_NAMESPACE_BEGIN

  template<class Result, class... ArgTypes>
  class TaskCancelable;

  template<class Result, class... ArgTypes>
  class TaskCancelable<Result(ArgTypes...)> : public ITaskCancelable {
  public:
    using Ptr = std::shared_ptr<TaskCancelable>;
    using FuncType = std::function<Result(ArgTypes...)>;

    ~TaskCancelable() = default;

    template<typename Func>
    TaskCancelable(Func &&task) {
      _strongTask = std::make_shared<FuncType>(std::forward<Func>(task));
      _weakTask = _strongTask;
    }

  public:
    void Cancel() override {
      _strongTask = nullptr;
    }

  public:
    operator bool() {
      return _strongTask && *_strongTask;
    }

    void operator=(std::nullptr_t) {
      _strongTask = nullptr;
    }

    Result operator()(ArgTypes ...args) const {
      auto strongTask = _weakTask.lock();
      if (strongTask && *strongTask) {
        return (*strongTask)(std::forward<ArgTypes>(args)...);
      }
      return defaultValue<Result>();
    }

  private:
    template<typename T>
    static typename std::enable_if<std::is_void<T>::value, void>::type
    defaultValue() {}

    template<typename T>
    static typename std::enable_if<std::is_integral<T>::value, T>::type
    defaultValue() { return 0; }

    template<typename T>
    static typename std::enable_if<std::is_pointer<T>::value, T>::type
    defaultValue() { return nullptr; }

  protected:
    std::weak_ptr<FuncType> _weakTask;
    std::shared_ptr<FuncType> _strongTask;
  };


RD_NAMESPACE_END

#endif //RENDU_TASK_CANCELABLE_H
