/*
* Created by boil on 2023/12/17.
*/

#ifndef RENDU_RD_TASK_RESULT_H
#define RENDU_RD_TASK_RESULT_H

#include <exception>

template<typename T>
struct RDTaskResult {
  
  explicit RDTaskResult() = default;

  explicit RDTaskResult(T &&value) : _value(value) {}

  explicit RDTaskResult(std::exception_ptr &&exception_ptr) : _exception_ptr(exception_ptr) {}

  T get_or_throw() {
    if (_exception_ptr) {
      std::rethrow_exception(_exception_ptr);
    }
    return _value;
  }

private:
  T _value{};
  std::exception_ptr _exception_ptr;
};


//template<>
//struct RDTaskResult<void> {
//
//  explicit RDTaskResult() = default;
//
//  explicit RDTaskResult(std::exception_ptr &&exception_ptr) : _exception_ptr(exception_ptr) {}
//
//  void get_or_throw() {
//    if (_exception_ptr) {
//      std::rethrow_exception(_exception_ptr);
//    }
//  }
//
//private:
//  std::exception_ptr _exception_ptr;
//};


#endif//RENDU_RD_TASK_RESULT_H
