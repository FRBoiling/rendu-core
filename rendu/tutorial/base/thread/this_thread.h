/*
* Created by boil on 2024/6/28.
*/

#ifndef RENDU_EXAMPLE_BASE_THREAD_THIS_THREAD_H_
#define RENDU_EXAMPLE_BASE_THREAD_THIS_THREAD_H_

#include <atomic>
#include <spdlog/spdlog.h>

namespace ThisThread {
  extern std::atomic<unsigned long> next_id;
  // 线程局部存储当前线程的ID
  extern thread_local unsigned long current_thread_id;
  static unsigned long GetNextId(){
    return next_id++;
  }

  static unsigned long GetCurrentThreadId(){
    return current_thread_id;
  }

  static void SetCurrentThreadId(unsigned long id){
    current_thread_id = id;
  }
};


#endif//RENDU_EXAMPLE_BASE_THREAD_THIS_THREAD_H_
