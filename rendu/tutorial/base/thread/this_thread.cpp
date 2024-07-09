/*
* Created by boil on 2024/6/28.
*/

#include "this_thread.h"

namespace ThisThread{
  std::atomic<unsigned long> next_id = 1;
  // 线程局部存储当前线程的ID
  thread_local unsigned long current_thread_id = 0;

}
