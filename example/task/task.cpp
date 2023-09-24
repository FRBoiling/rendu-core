///*
//* Created by boil on 2023/9/29.
//*/
//
//#include "task.h"
//
//RD_NAMESPACE_BEGIN
//    void task_fork::await_suspend(std::coroutine_handle<> coro) {
//      std::thread t(
//          [=] {
//            std::cout << "New thread id " << std::this_thread::get_id() << std::endl;
//            coro.resume();
//          });
//      t.detach();
//    }
//
//RD_NAMESPACE_END