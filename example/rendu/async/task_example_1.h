/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_TASK_EXAMPLE_1_H
#define RENDU_TASK_EXAMPLE_1_H

#include <iostream>
#include <thread>
#include <future>
#include <functional>
#include <exception>
#include <utility>

// A class that mimics the C# Task<TResult> class
template <typename TResult>
class CppTask {
public:
  // Constructor that takes a function object that returns TResult
  explicit CppTask(std::function<TResult()> func,int n) : _func(std::move(func)), _started(false), _completed(false), _faulted(false) {}

  // Start the asynchronous operation
  void Start() {
    // Check if the task has already started
    if (_started) {
      throw std::runtime_error("The task has already started.");
    }
    // Set the started flag to true
    _started = true;
    // Create a thread to execute the function object
    _thread = std::thread([this] {
      try {
        // Set the promise value with the function object result
        _promise.set_value(_func());
      } catch (...) {
        // Set the promise exception with the current exception
        _promise.set_exception(std::current_exception());
      }
    });
    // Detach the thread
    _thread.detach();
  }

  // Wait for the asynchronous operation to complete
  void Wait() {
    // Check if the task has started
    if (!_started) {
      throw std::runtime_error("The task has not started yet.");
    }
    // Get the future from the promise
    auto future = _promise.get_future();
    // Wait for the future to be ready
    future.wait();
    // Set the completed flag to true
    _completed = true;
    // Check if the future has an exception
    if (future.valid()) {
      try {
        // Get the result from the future
        future.get();
      } catch (...) {
        // Set the faulted flag to true
        _faulted = true;
        // Set the exception pointer with the current exception
        _exception = std::current_exception();
      }
    }
  }

  // Get the result of the asynchronous operation
  TResult Result() {
    // Check if the task has completed
    if (!_completed) {
      // Wait for the task to complete
      Wait();
    }
    // Check if the task has faulted
    if (_faulted) {
      // Rethrow the exception
      std::rethrow_exception(_exception);
    }
    // Get the future from the promise
    auto future = _promise.get_future();
    // Return the result from the future
    return future.get();
  }

  // Check if the task has completed
  bool IsCompleted() {
    return _completed;
  }

  // Check if the task has faulted
  bool IsFaulted() {
    return _faulted;
  }

  // Get the exception of the task
  std::exception_ptr Exception() {
    return _exception;
  }

private:
  // The function object to execute
  std::function<TResult()> _func;
  // The promise to store the result or the exception
  std::promise<TResult> _promise;
  // The thread to run the function object
  std::thread _thread;
  // The flag to indicate if the task has started
  bool _started;
  // The flag to indicate if the task has completed
  bool _completed;
  // The flag to indicate if the task has faulted
  bool _faulted;
  // The exception pointer to store the exception
  std::exception_ptr _exception;
};

// A function that returns the factorial of a number
int factorial(int n) {
  if (n < 0) {
    throw std::invalid_argument("n must be non-negative");
  }
  int res = 1;
  for (int i = 1; i <= n; i++) {
    res *= i;
  }
  return res;
}

// A function that returns the sum of two numbers
int sum(int a, int b) {
  return a + b;
}

// A function that throws an exception
int thrower() {
  throw std::runtime_error("Something went wrong");
}

int task1_main() {
  // Create a task that calculates the factorial of 5
  CppTask<int> task1(factorial, 5);
  // Start the task
  task1.Start();
  // Get the result of the task
  std::cout << "The factorial of 5 is " << task1.Result() << std::endl;

  // Create a task that calculates the sum of 3 and 4
  CppTask<int> task2(std::bind(sum, 3, 4));
  // Start the task
  task2.Start();
  // Get the result of the task
  std::cout << "The sum of 3 and 4 is " << task2.Result() << std::endl;

  // Create a task that throws an exception
  CppTask<int> task3(thrower);
  // Start the task
  task3.Start();
  // Try to get the result of the task
  try {
    std::cout << "The result of the thrower is " << task3.Result() << std::endl;
  } catch (const std::exception& e) {
    std::cout << "The task threw an exception: " << e.what() << std::endl;
  }

  return 0;
}



#endif//RENDU_TASK_EXAMPLE_1_H
