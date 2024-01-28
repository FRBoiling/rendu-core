/*
* Created by boil on 2023/12/27.
*/

#ifndef RENDU_TASK_H
#define RENDU_TASK_H

#include "detached_coroutine.h"
#include "log.h"
#include "task_define.h"
#include "via_coroutine.h"
#include <atomic>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <variant>
#include "try.h"

template <typename T>
class Task;

// In the middle of the execution of one coroutine, if we want to give out the
// rights to execute back to the executor, to make it schedule other tasks to
// execute, we could write:
//
// ```C++
//  co_await Yield{};
// ```
//
// This would suspend the executing coroutine.
struct Yield {};

template <typename T = void>
struct TaskLocals {};

namespace detail {
  template <class, typename OAlloc, bool Para>
  struct CollectAllAwaiter;

  template <bool Para, template <typename> typename TaskType, typename... Ts>
  struct CollectAllVariadicAwaiter;

  template <typename TaskType, typename IAlloc>
  struct CollectAnyAwaiter;

  template <template <typename> typename TaskType, typename... Ts>
  struct CollectAnyVariadicAwaiter;

}  // namespace detail

namespace detail {

  class TaskPromiseBase {
  public:
    // Resume the caller waiting to the current coroutine. Note that we need
    // destroy the frame for the current coroutine explicitly. Since after
    // FinalAwaiter, The current coroutine should be suspended and never to
    // resume. So that we couldn't expect it to release it self any more.
    struct FinalAwaiter {
      bool await_ready() const noexcept { return false; }
      template <typename PromiseType>
      auto await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
        static_assert(
            std::is_base_of<TaskPromiseBase, PromiseType>::value,
            "the final awaiter is only allowed to be called by Task");

        return h.promise()._continuation;
      }
      void await_resume() noexcept {}
    };

    struct YieldAwaiter {
      YieldAwaiter(Executor* executor) : _executor(executor) {}
      bool await_ready() const noexcept { return false; }
      template <typename PromiseType>
      void await_suspend(std::coroutine_handle<PromiseType> handle) {
        static_assert(
            std::is_base_of<TaskPromiseBase, PromiseType>::value,
            "'co_await Yield' is only allowed to be called by Task");

        logicAssert(_executor,
                    "Yielding is only meaningful with an executor!");
        _executor->schedule(std::move(handle));
      }
      void await_resume() noexcept {}

    private:
      Executor* _executor;
    };

  public:
    TaskPromiseBase() noexcept : _executor(nullptr), _lazy_local(nullptr) {}
    // Lazily started, coroutine will not execute until first resume() is called
    std::suspend_always initial_suspend() noexcept { return {}; }
    FinalAwaiter final_suspend() noexcept { return {}; }

    template <typename Awaitable>
    auto await_transform(Awaitable&& awaitable) {
      // See CoAwait.h for details.
      return detail::coAwait(_executor, std::forward<Awaitable>(awaitable));
    }

    auto await_transform(CurrentExecutor) {
      return ReadyAwaiter<Executor*>(_executor);
    }

    template <typename T>
    auto await_transform(TaskLocals<T>) {
      return ReadyAwaiter<T*>(static_cast<T*>(_lazy_local));
    }

    auto await_transform(Yield) { return YieldAwaiter(_executor); }

    /// IMPORTANT: _continuation should be the first member due to the
    /// requirement of dbg script.
    std::coroutine_handle<> _continuation;
    Executor* _executor;
    void* _lazy_local;
  };

  template <typename T>
  class TaskPromise : public TaskPromiseBase {
  public:
    static_assert(alignof(T) <= alignof(::max_align_t),
                  "async_simple doesn't allow Task with over aligned object");

    TaskPromise() noexcept {}
    ~TaskPromise() noexcept {}

    Task<T> get_return_object() noexcept;

    static Task<T> get_return_object_on_allocation_failure() noexcept;

    template <typename V>
    void return_value(V&& value) noexcept(
        std::is_nothrow_constructible_v<
            T, V&&>) requires std::is_convertible_v<V&&, T> {
      _value.template emplace<T>(std::forward<V>(value));
    }
    void unhandled_exception() noexcept {
      _value.template emplace<std::exception_ptr>(std::current_exception());
    }

  public:
    T& result() & {
      if (std::holds_alternative<std::exception_ptr>(_value))
          std::rethrow_exception(std::get<std::exception_ptr>(_value));
      assert(std::holds_alternative<T>(_value));
      return std::get<T>(_value);
    }
    T&& result() && {
      if (std::holds_alternative<std::exception_ptr>(_value))
          std::rethrow_exception(std::get<std::exception_ptr>(_value));
      assert(std::holds_alternative<T>(_value));
      return std::move(std::get<T>(_value));
    }

    Try<T> tryResult() noexcept {
      if (std::holds_alternative<std::exception_ptr>(_value))
        return Try<T>(std::get<std::exception_ptr>(_value));
      else {
        assert(std::holds_alternative<T>(_value));
        return Try<T>(std::move(std::get<T>(_value)));
      }
    }

    std::variant<std::monostate, T, std::exception_ptr> _value;
  };

  template <>
  class TaskPromise<void> : public TaskPromiseBase {
  public:
    TaskPromise() noexcept {}
    ~TaskPromise() noexcept {}

    Task<void> get_return_object() noexcept;
    static Task<void> get_return_object_on_allocation_failure() noexcept;

    void return_void() noexcept {}
    void unhandled_exception() noexcept {
      _exception = std::current_exception();
    }

    void result() {
      if (_exception != nullptr)
         std::rethrow_exception(_exception);
    }
    Try<void> tryResult() noexcept { return Try<void>(_exception); }

  public:
    std::exception_ptr _exception{nullptr};
  };

}  // namespace detail

template <typename T>
class RescheduleTask;

namespace detail {

  template <typename T>
  struct TaskAwaiterBase {
    using Handle = std::coroutine_handle<detail::TaskPromise<T>>;
    Handle _handle;

    TaskAwaiterBase(TaskAwaiterBase& other) = delete;
    TaskAwaiterBase& operator=(TaskAwaiterBase& other) = delete;

    TaskAwaiterBase(TaskAwaiterBase&& other)
        : _handle(std::exchange(other._handle, nullptr)) {}

    TaskAwaiterBase& operator=(TaskAwaiterBase&& other) {
      std::swap(_handle, other._handle);
      return *this;
    }

    TaskAwaiterBase(Handle coro) : _handle(coro) {}
    ~TaskAwaiterBase() {
      if (_handle) {
        _handle.destroy();
        _handle = nullptr;
      }
    }

    bool await_ready() const noexcept { return false; }

    auto awaitResume() {
      if constexpr (std::is_void_v<T>) {
        _handle.promise().result();
        // We need to destroy the handle expclictly since the awaited
        // coroutine after symmetric transfer couldn't release it self any
        // more.
        _handle.destroy();
        _handle = nullptr;
      } else {
        auto r = std::move(_handle.promise()).result();
        _handle.destroy();
        _handle = nullptr;
        return r;
      }
    }

    Try<T> awaitResumeTry() noexcept {
      Try<T> ret = _handle.promise().tryResult();
      _handle.destroy();
      _handle = nullptr;
      return ret;
    }
  };

  template <typename T, bool reschedule>
  class TaskBase {
  public:
    using promise_type = detail::TaskPromise<T>;
    using Handle = std::coroutine_handle<promise_type>;
    using ValueType = T;

    struct AwaiterBase : public detail::TaskAwaiterBase<T> {
      using Base = detail::TaskAwaiterBase<T>;
      AwaiterBase(Handle coro) : Base(coro) {}

      template <typename PromiseType>
      auto await_suspend(std::coroutine_handle<PromiseType>
                                       continuation) noexcept(!reschedule) {
        static_assert(
            std::is_base_of<TaskPromiseBase, PromiseType>::value ||
                std::is_same_v<detail::DetachedCoroutine::promise_type,
                               PromiseType>,
            "'co_await Task' is only allowed to be called by Task or "
            "DetachedCoroutine");

        // current coro started, caller becomes my continuation
        this->_handle.promise()._continuation = continuation;
        if constexpr (std::is_base_of<TaskPromiseBase,
                                      PromiseType>::value) {
          this->_handle.promise()._lazy_local =
              continuation.promise()._lazy_local;
        }
        return awaitSuspendImpl();
      }

    private:
      auto awaitSuspendImpl() noexcept(!reschedule) {
        if constexpr (reschedule) {
          // executor schedule performed
          auto& pr = this->_handle.promise();
          logicAssert(pr._executor, "RescheduleTask need executor");
          pr._executor->schedule(this->_handle);
        } else {
          return this->_handle;
        }
      }
    };

    struct TryAwaiter : public AwaiterBase {
      TryAwaiter(Handle coro) : AwaiterBase(coro) {}
      Try<T> await_resume() noexcept {
        return AwaiterBase::awaitResumeTry();
      };

      auto coAwait(Executor* ex) {
        if constexpr (reschedule) {
          logicAssert(false,
                      "RescheduleTask should be only allowed in "
                      "DetachedCoroutine");
        }
        // derived lazy inherits executor
        this->_handle.promise()._executor = ex;
        return std::move(*this);
      }
    };

    struct ValueAwaiter : public AwaiterBase {
      ValueAwaiter(Handle coro) : AwaiterBase(coro) {}
      T await_resume() { return AwaiterBase::awaitResume(); }
    };

    ~TaskBase() {
      if (_coro) {
        _coro.destroy();
        _coro = nullptr;
      }
    };
    explicit TaskBase(Handle coro) noexcept : _coro(coro) {}
    TaskBase(TaskBase&& other) noexcept : _coro(std::move(other._coro)) {
      other._coro = nullptr;
    }

    TaskBase(const TaskBase&) = delete;
    TaskBase& operator=(const TaskBase&) = delete;

    Executor* getExecutor() { return _coro.promise()._executor; }
    
    bool isReady() const { return !_coro || _coro.done(); }

    auto operator co_await() {
      return ValueAwaiter(std::exchange(_coro, nullptr));
    }

    auto coAwaitTry() { return TryAwaiter(std::exchange(_coro, nullptr)); }

    void Start(){
      _coro.resume();
    }

  protected:
    Handle _coro;

    template <class, typename OAlloc, bool Para>
    friend struct detail::CollectAllAwaiter;

    template <bool, template <typename> typename, typename...>
    friend struct detail::CollectAllVariadicAwaiter;

    template <typename TaskType, typename IAlloc>
    friend struct detail::CollectAnyAwaiter;

    template <template <typename> typename TaskType, typename... Ts>
    friend struct detail::CollectAnyVariadicAwaiter;
  };

}  // namespace detail

// Task is a coroutine task which would be executed lazily.
// The user who wants to use Task should declare a function whose return type
// is Task<T>. T is the type you want the function to return originally.
// And if the function doesn't want to return any thing, use Task<>.
//
// Then in the function, use co_return instead of return. And use co_await to
// wait things you want to wait. For example:
//
// ```C++
//  // Return 43 after 10s.
//  Task<int> foo() {
//     co_await sleep(10s);
//     co_return 43;
// }
// ```
//
// To get the value wrapped in Task, we could co_await it like:
//
// ```C++
//  Task<int> bar() {
//      // This would return the value foo returned.
//      co_return co_await foo();
// }
// ```
//
// If we don't want the caller to be a coroutine too, we could use Task::start
// to get the value asynchronously.
//
// ```C++
// void foo_use() {
//     foo().start([](Try<int> &&value){
//         std::cout << "foo: " << value.value() << "\n";
//     });
// }
// ```
//
// When the foo gets its value, the value would be passed to the lambda in
// Task::start().
//
// If the user wants to get the value synchronously, he could use
// async_simple::coro::syncAwait.
//
// ```C++
// void foo_use2() {
//     auto val = async_simple::coro::syncAwait(foo());
//     std::cout << "foo: " << val << "\n";
// }
// ```
//
// There is no executor instance in a Task. To specify an executor to schedule
// the execution of the Task and corresponding Task tasks inside, user could use
// `Task::via` to assign an executor for this Task. `Task::via` would return a
// RescheduleTask. User should use the returned RescheduleTask directly. The
// Task which called `via()` shouldn't be used any more.
//
// If Task is co_awaited directly, sysmmetric transfer would happend. That is,
// the stack frame for current caller would be released and the lazy task would
// be resumed directly. So the user needn't to worry about the stack overflow.
//
// The co_awaited Task shouldn't be accessed any more.
//
// When a Task is co_awaited, if there is any exception happened during the
// process, the co_awaited expression would throw the exception happened. If the
// user does't want the co_await expression to throw an exception, he could use
// `Task::coAwaitTry`. For example:
//
//  ```C++
//      Try<int> res = co_await foo().coAwaitTry();
//      if (res.hasError())
//          std::cout << "Error happend.\n";
//      else
//          std::cout << "We could get the value: " << res.value() << "\n";
// ```
//
// If any awaitable wants to derive the executor instance from its caller, it
// should implement `coAwait(Executor*)` member method. Then the caller would
// pass its executor instance to the awaitable.
template <typename T = void>
class [[nodiscard]] Task
    : public detail::TaskBase<T, /*reschedule=*/false> {
  using Base = detail::TaskBase<T, false>;

public:
  using Base::Base;

  // Bind an executor to a Task, and convert it to RescheduleTask.
  // You can only call via on rvalue, i.e. a Task is not accessible after
  // via() called.
  RescheduleTask<T> via(Executor* ex) && {
    logicAssert(this->_coro.operator bool(),
                "Task do not have a coroutine_handle "
                "Maybe the allocation failed or you're using a used Task");

    this->_coro.promise()._executor = ex;
    return RescheduleTask<T>(std::exchange(this->_coro, nullptr));
  }

  // Bind an executor only. Don't re-schedule.
  //
  // Users shouldn't use `setEx` directly. `setEx` is designed
  // for internal purpose only. See uthread/Await.h/await for details.
  Task<T> setEx(Executor* ex) && {
    logicAssert(this->_coro.operator bool(),
                "Task do not have a coroutine_handle "
                "Maybe the allocation failed or you're using a used Task");
    this->_coro.promise()._executor = ex;
    return Task<T>(std::exchange(this->_coro, nullptr));
  }

  auto coAwait(Executor* ex) {
    logicAssert(this->_coro.operator bool(),
                "Task do not have a coroutine_handle "
                "Maybe the allocation failed or you're using a used Task");

    // derived lazy inherits executor
    this->_coro.promise()._executor = ex;
    return typename Base::ValueAwaiter(std::exchange(this->_coro, nullptr));
  }


private:
  friend class RescheduleTask<T>;
};

// A RescheduleTask is a Task with an executor. The executor of a RescheduleTask
// wouldn't/shouldn't be nullptr. So we needn't check it.
//
// The user couldn't/shouldn't declare a coroutine function whose return type is
// RescheduleTask. The user should get a RescheduleTask by a call to
// `Task::via(Executor)` only.
//
// Different from Task, when a RescheduleTask is co_awaited/started/syncAwaited,
// the RescheduleTask wouldn't be executed immediately. The RescheduleTask would
// submit a task to resume the corresponding Task task to the executor. Then the
// executor would execute the Task task later.
template <typename T = void>
class [[nodiscard]] RescheduleTask
    : public detail::TaskBase<T, /*reschedule=*/true> {
  using Base = detail::TaskBase<T, true>;

public:
  void detach() {
    this->start([](auto&& t) {
      if (t.hasError()) {
        std::rethrow_exception(t.getException());
      }
    });
  }

  RescheduleTask<T> setTaskLocal(void* lazy_local) && {
    logicAssert(this->_coro.operator bool(),
                "Task do not have a coroutine_handle "
                "Maybe the allocation failed or you're using a used Task");
    this->_coro.promise()._lazy_local = lazy_local;
    return RescheduleTask<T>(std::exchange(this->_coro, nullptr));
  }

  [[deprecated(
      "RescheduleTask should be only allowed in DetachedCoroutine")]] auto
  operator co_await() {
    return Base::operator co_await();
  }

private:
  using Base::Base;
};

template <typename T>
inline Task<T> detail::TaskPromise<T>::get_return_object() noexcept {
  return Task<T>(Task<T>::Handle::from_promise(*this));
}

inline Task<void> detail::TaskPromise<void>::get_return_object() noexcept {
  return Task<void>(Task<void>::Handle::from_promise(*this));
}

/// Why do we want to introduce `get_return_object_on_allocation_failure()`?
/// Since a coroutine will be roughly converted to:
///
/// ```C++
/// void *frame_addr = ::operator new(required size);
/// __promise_ = new (frame_addr) __promise_type(...);
/// __return_object_ = __promise_.get_return_object();
/// co_await __promise_.initial_suspend();
/// try {
///     function-body
/// } catch (...) {
///     __promise_.unhandled_exception();
/// }
/// co_await __promise_.final_suspend();
/// ```
///
/// Then we can find that the coroutine should be nounwind (noexcept) naturally
/// if the constructor of the promise_type, the get_return_object() function,
/// the initial_suspend, the unhandled_exception(), the final_suspend and the
/// allocation function is noexcept.
///
/// For the specific coroutine type, Task, all the above except the allocation
/// function is noexcept. So that we can make every Task function noexcept
/// naturally if we make the allocation function nothrow. This is the reason why
/// we want to introduce `get_return_object_on_allocation_failure()` to Task.
///
/// Note that the optimization may not work in some platforms due the ABI
/// limitations. Since they need to consider the case that the destructor of an
/// exception can throw exceptions.
template <typename T>
inline Task<T>
detail::TaskPromise<T>::get_return_object_on_allocation_failure() noexcept {
  return Task<T>(typename Task<T>::Handle(nullptr));
}

inline Task<void>
detail::TaskPromise<void>::get_return_object_on_allocation_failure() noexcept {
  return Task<void>(Task<void>::Handle(nullptr));
}

#endif//RENDU_TASK_H
