///*
//* Created by boil on 2023/10/10.
//*/
//
//#ifndef RENDU_MY_TASK_H
//#define RENDU_MY_TASK_H
//
//#include <coroutine>
//#include <memory>
//#include <thread>
//#include <atomic>
//#include <iostream>
//// A result type that can hold a value or an exception
//template<typename T>
//struct Result {
//  T value;
//  std::exception_ptr exception;
//
//  Result(T value) : value(value) {}
//
//  Result(std::exception_ptr exception) : exception(exception) {}
//};
//
//// A promise type that supports co_return and co_await
//template<typename T>
//struct MyTask {
//  struct promise_type {
//    Result<T> result;
//    std::coroutine_handle<> waiter;
//
//    bool completed; // Flag to track completion status
//
//    promise_type() : result(T()), completed(false) {}
//
//    MyTask get_return_object() {
//      return MyTask(std::coroutine_handle<promise_type>::from_promise(*this));
//    }
//
//    std::suspend_always initial_suspend() {
//      return {};
//    }
//
//    std::suspend_always final_suspend() noexcept {
//      if (waiter) {
//        waiter.resume();
//      }
//      return {};
//    }
//
//    void return_value(T value) {
//      result = Result<T>(value);
//      completed = true; // Mark coroutine as completed
//    }
//
//    void unhandled_exception() {
//      result = Result<T>(std::current_exception());
//      completed = true; // Mark coroutine as completed
//    }
//
//    template<typename U>
//    auto await_transform(MyTask<U> task) {
//      struct Awaiter {
//        MyTask<U> task;
//
//        Awaiter(MyTask<U> task) : task(task) {}
//
//        bool await_ready() {
//          return false;
//        }
//
//        void await_suspend(std::coroutine_handle<promise_type> coro) {
//          task.coro.promise().waiter = coro;
//        }
//
//        U await_resume() {
//          if (task.coro.promise().result.exception) {
//            std::rethrow_exception(task.coro.promise().result.exception);
//          }
//          return task.coro.promise().result.value;
//        }
//      };
//      return Awaiter(task);
//    }
//  };
//
//  std::coroutine_handle<promise_type> coro;
//
//  MyTask(std::coroutine_handle<promise_type> coro) : coro(coro) {}
//
//  ~MyTask() {
//    if (coro && !coro.done()) { // Check if coroutine has completed
//      coro.destroy();
//    }
//  }
//
//  // A blocking function to get the result
//  T get_result() {
//    if (!coro || coro.done()) { // Check if coroutine has completed
//      throw std::runtime_error("Coroutine has already completed");
//    }
//    coro.resume();
//    if (coro.promise().result.exception) {
//      std::rethrow_exception(coro.promise().result.exception);
//    }
//    return coro.promise().result.value;
//  }
//
//  // A callback function to get the value
//  template<typename F>
//  void then(F f) {
//    if (!coro || coro.done()) { // Check if coroutine has completed
//      throw std::runtime_error("Coroutine has already completed");
//    }
//    struct Awaiter {
//      F f;
//
//      Awaiter(F f) : f(f) {}
//
//      bool await_ready() {
//        return false;
//      }
//
//      void await_suspend(std::coroutine_handle<>) {}
//
//      void await_resume() {
//        f();
//      }
//    };
//    coro.resume();
//    Awaiter(f).await_resume();
//  }
//
//  // A callback function to get the exception
//  template<typename F>
//  void catching(F f) {
//    if (!coro || coro.done()) { // Check if coroutine has completed
//      throw std::runtime_error("Coroutine has already completed");
//    }
//    struct Awaiter {
//      F f;
//
//      Awaiter(F f) : f(f) {}
//
//      bool await_ready() {
//        return false;
//      }
//
//      void await_suspend(std::coroutine_handle<>) {}
//
//      void await_resume() {
//        f();
//      }
//    };
//    coro.resume();
//    if (coro.promise().result.exception) {
//      Awaiter(f).await_resume();
//    }
//  }
//};
//
//// A sample Async function that returns a MyTask
//MyTask<int> async_add(int a, int b) {
//  std::cout << "async_add 1:\n";
//  co_return a + b;
//  std::cout << "async_add 2:\n";
//}
//
//// A sample Async function that throws an exception
//MyTask<int> async_throw(int a, int b) {
//  throw std::runtime_error("Oops");
//  co_return a + b;
//}
//
//
//void my_task_test(){
//  auto task = async_add(10, 20);
//  std::cout << "after 1:\n";
//  // Use get_result to block and get the value
//  try {
//    auto result = task.get_result();
//    std::cout << "Result: " << result << "\n";
//  } catch (const std::exception &e) {
//    std::cout << "Exception: " << e.what() << "\n";
//  }
//
////  // Use then to get the value asynchronously
////  task.then([] {
////    std::cout << "Done\n";
////  });
////
////  // Use catching to get the exception asynchronously
////  task.catching([] {
////    std::cout << "Error\n";
////  });
//}
//
//#endif //RENDU_MY_TASK_H
