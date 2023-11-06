///*
//* Created by boil on 2023/10/10.
//*/
//
//#ifndef RENDU_TASK_H
//#define RENDU_TASK_H
//
//#include "define.h"
//#include "coroutine/coroutine.h"
//#include "task_promise.h"
//#include "task_fwd.h"
//
//RD_NAMESPACE_BEGIN
//
//    enum class task_context_state : int {
//      Init, // task init
//      Complete, // task complete by sub_thread
//      Awaiting, // there is coroutine awaiting me
//      ParentCoroSet, // parent coroutine handler is set, sub_thread could resume on parent remaining content
//    };
//
//    class task_fork {
//    public:
//      bool await_ready() {
//        return false;
//      }
//
//      void await_suspend(std::coroutine_handle<> coro);
//
//      void await_resume() {
//      }
//    };
//
//
//    template<typename T = void>
//    class Task {
//    public:
//      using promise_type = task_promise<T>;
//      using value_type = T;
//
//    private:
//
//      struct promise_type {
//        promise_type() {
//          std::cout << "Promise +" << std::endl;
//        }
//
//        ~promise_type() {
//          std::cout << "Promise -" << std::endl;
//        }
//
//        auto initial_suspend() noexcept {
//          return std::suspend_never();
//        }
//
//        auto final_suspend() noexcept {
//          task_context_state exp = task_context_state::Init;
//          if (!_ctx->_state.compare_exchange_strong(exp, task_context_state::Complete)) {
//            while (_ctx->_state.load() != task_context_state::ParentCoroSet) {
//              std::this_thread::yield();
//            }
//            _ctx->_parent_coro.resume();
//          }
//          return std::suspend_always();
//        }
//
//        Task<T> get_return_object() {
//          auto ctx = std::make_shared<TaskContext>();
//          ctx->_coro = std::coroutine_handle<promise_type>::from_promise(*this);
//          ctx->_parent_coro = nullptr;
//          ctx->_state.store(task_context_state::Init);
//
//          _ctx = ctx;
//          return Task<T>(ctx);
//        }
//
//        void return_value(T value) {
//          _result = value;
//        }
//
//        void unhandled_exception() noexcept {
//          return;
//        }
//
//        std::shared_ptr<TaskContext> _ctx;
//        T _result;
//      };
//
//      Task() {
//        std::cout << "Task +" << std::endl;
//      };
//
//      ~Task() {
//        std::cout << "Task -" << std::endl;
//      };
//
//      Task(std::shared_ptr<TaskContext> ctx) :
//          _ctx(ctx) {
//        std::cout << "Task +" << std::endl;
//      }
//
//      Task(const Task &rhs) :
//          _ctx(rhs._ctx) {
//        std::cout << "Task +" << std::endl;
//      }
//
//      bool await_ready() {
//        task_context_state exp = task_context_state::Init;
//        if (_ctx->_state.compare_exchange_strong(exp, task_context_state::Awaiting)) {
//          return false;
//        } else {
//          return true;
//        }
//      }
//
//      void await_suspend(std::coroutine_handle<promise_type> coro) {
//        _ctx->_parent_coro = coro;
//        _ctx->_state.store(task_context_state::ParentCoroSet);
//      }
//
//      void await_resume() {
//      }
//
//    public:
//      std::shared_ptr<TaskContext> _ctx;
//
//      bool is_complete() {
//        return _ctx->_state == task_context_state::Complete;
//      }
//
//      T result() {
//        auto result = _ctx->_coro.promise()._result;
//
//        return result;
//      }
//
//    };
//
//    template<>
//    class Task<void> {
//    public:
//      struct TaskContext {
//        std::coroutine_handle<> _coro;
//        std::coroutine_handle<> _parent_coro;
//        std::atomic<task_context_state> _state;
//      };
//
//      struct promise_type {
//        promise_type() {
//          std::cout << "void Promise +" << std::endl;
//        }
//
//        ~promise_type() {
//          std::cout << "void Promise -" << std::endl;
//        }
//
//        auto initial_suspend() noexcept {
//          return std::suspend_never();
//        }
//
//        auto final_suspend() noexcept {
//          task_context_state exp = task_context_state::Init;
//          if (!_ctx->_state.compare_exchange_strong(exp, task_context_state::Complete)) {
//            while (_ctx->_state.load() != task_context_state::ParentCoroSet) {
//              std::this_thread::yield();
//            }
//            _ctx->_parent_coro.resume();
//          }
//          return std::suspend_never();
//        }
//
//        Task<void> get_return_object() {
//          auto ctx = std::make_shared<TaskContext>();
//          ctx->_coro = std::coroutine_handle<promise_type>::from_promise(*this);
//          ctx->_parent_coro = nullptr;
//          ctx->_state.store(task_context_state::Init);
//
//          _ctx = ctx;
//          return Task(ctx);
//        }
//
//        void return_void() {
//        }
//
//        void unhandled_exception() noexcept {
//          return;
//        }
//
//        std::shared_ptr<TaskContext> _ctx;
//      };
//
//      Task() {
//        std::cout << "void Task +" << std::endl;
//      };
//
//      ~Task() {
//        std::cout << "void Task -" << std::endl;
//      };
//
//      Task(std::shared_ptr<TaskContext> ctx) :
//          _ctx(ctx) {
//        std::cout << "void Task +" << std::endl;
//      }
//
//      Task(const Task &rhs) :
//          _ctx(rhs._ctx) {
//        std::cout << "void Task +" << std::endl;
//      }
//
//      bool await_ready() {
//        task_context_state exp = task_context_state::Init;
//        if (_ctx->_state.compare_exchange_strong(exp, task_context_state::Awaiting)) {
//          return false;
//        } else {
//          return true;
//        }
//      }
//
//      void await_suspend(std::coroutine_handle<> coro) {
//        _ctx->_parent_coro = coro;
//        _ctx->_state.store(task_context_state::ParentCoroSet);
//      }
//
//      void await_resume() {
//      }
//
//    public:
//      std::shared_ptr<TaskContext> _ctx;
//
//      bool is_complete() {
//        return _ctx->_state == task_context_state::Complete;
//      }
//
//    };
//
//RD_NAMESPACE_END
//
//#include "task_completion_source.h"
//
//#endif //RENDU_TASK_H
