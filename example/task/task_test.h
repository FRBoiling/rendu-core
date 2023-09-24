#include "task/task.h"
#include "task/sync_wait.h"
#include "static_thread_pool.h"
#include <thread>

namespace task_test {

    using namespace rendu;

    class TestTask {
    public:
      static TestTask &Instance() {
        static TestTask instance;
        return instance;
      }

    public:

      Task<int> func_A(int &x) {
//    co_await task_fork();
        std::cout << __FUNCTION__ << " #1 my thread id is " << std::this_thread::get_id() << std::endl;

        auto future = func_B(x);

        std::cout << __FUNCTION__ << " #2 my thread id is " << std::this_thread::get_id() << std::endl;

//        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto result = sync_wait(future) ;

        std::cout << __FUNCTION__ << " #3 my thread id is " << std::this_thread::get_id() << std::endl;
        co_return result;
      }

      Task<int> func_B(int &x) {
        std::cout << __FUNCTION__ << " #1 my thread id is " << std::this_thread::get_id() << std::endl;

//    co_await task_fork(); // 如果这是一个耗时函数,在函数开头调用 task_fork 产生一个线程
        x = x * x;
//        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        std::cout << __FUNCTION__ << " #2 my thread id is " << std::this_thread::get_id() << std::endl;

        co_return x;
      }


      Task<void> func_A1(int &x) {
//    co_await task_fork();
        std::cout << __FUNCTION__ << " #1 my thread id is " << std::this_thread::get_id() << std::endl;

        auto future = func_B1(x);

        std::cout << __FUNCTION__ << " #2 my thread id is " << std::this_thread::get_id() << std::endl;

//  std::this_thread::sleep_for(std::chrono::milliseconds(500));
        co_await future;

        std::cout << __FUNCTION__ << " #3 my thread id is " << std::this_thread::get_id() << std::endl;

        co_return;
      }

      Task<void> func_B1(int &x) {
        std::cout << __FUNCTION__ << " #1 my thread id is " << std::this_thread::get_id() << std::endl;

//    co_await task_fork(); // 如果这是一个耗时函数,在函数开头调用 task_fork 产生一个线程
        x = x * x;
//    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        std::cout << __FUNCTION__ << " #2 my thread id is " << std::this_thread::get_id() << std::endl;

        co_return;
      }

    };

    void func_Main(int input) {
      std::cout << __FUNCTION__ << "#1 main  thread id is " << std::this_thread::get_id() << std::endl;
      static_thread_pool threadPool{4};
      TestTask::Instance().func_A(input);

      std::cout << __FUNCTION__ << "#2 main thread id is " << std::this_thread::get_id() << std::endl;

//      while (true) {
//        if (t.is_ready()) {
//          std::cout << __FUNCTION__ << " 1 loop thread id is " << std::this_thread::get_id() << std::endl;
////          std::cout << "Result is " << decltype(t) << std::endl;
//          break;
//        }
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        std::cout << __FUNCTION__ << " 2 loop thread id is " << std::this_thread::get_id() << std::endl;
//      }
    }

    void func_Main1(int input) {
      std::cout << __FUNCTION__ << " main thread id is " << std::this_thread::get_id() << std::endl;

//  co_await task_fork(); // 如果这是一个耗时函数,在函数开头调用 task_fork 产生一个线程

      Task<> t = TestTask::Instance().func_A1(input);
      while (true) {
        if (t.is_ready()) {
          std::cout << __FUNCTION__ << " loop thread id is " << std::this_thread::get_id() << std::endl;
//      std::cout << "Result is " << t.result() << std::endl;
          break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << __FUNCTION__ << " loop thread id is " << std::this_thread::get_id() << std::endl;
      }
      return;
    }


    int task_test_fuc() {
      func_Main(5);
      while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
      }
      return 0;
    }
}
