#include <iostream>
#include <coroutine>
#include <thread>

using namespace std;

//// 协程函数，它是一个异步函数
//void myCoroutineFunction() {
//  cout << "协程开始执行" << endl;
//  yield();
//  cout << "协程执行完成" << endl;
//}
//
//int main() {
//  // 创建一个协程
//  auto coroutine = myCoroutineFunction();
//
//  // 在一个新线程中执行协程
//  std::thread t([]() {
//    coroutine.resume();
//  });
//
//  // 主线程中执行其他任务
//  cout << "主线程执行其他任务" << endl;
//
//  return 0;
//}
