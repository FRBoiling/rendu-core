/*
* Created by boil on 2024/1/10.
*/

#ifndef RENDU_TASK_RESULT_H
#define RENDU_TASK_RESULT_H


#include <exception>

template<typename T>
struct Result {

  explicit Result() = default;

  explicit Result(T &&value) : _value(value) {}

  explicit Result(std::exception_ptr &&exception_ptr) : _exception_ptr(exception_ptr) {}

  T GetOrThrow() {
    if (_exception_ptr) {
      std::rethrow_exception(_exception_ptr);
    }
    return _value;
  }

private:
  T _value{};
  std::exception_ptr _exception_ptr;
};

template<>
struct Result<void> {

  explicit Result() = default;
  explicit Result(std::exception_ptr &&exception_ptr) : _exception_ptr(exception_ptr) {}

  void GetOrThrow() {
    if (_exception_ptr) {
      std::rethrow_exception(_exception_ptr);
    }
  }

private:
  std::exception_ptr _exception_ptr;
};

#endif//RENDU_TASK_RESULT_H
