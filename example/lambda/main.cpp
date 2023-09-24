#include <iostream>
#include <functional>
#include "task/task.h"

using namespace rendu;

// 定义回调函数类型
using Callback = std::function<Task<void>()>;
// 执行回调函数
Task<int> PerformCallback(Callback callback) {
  // 执行回调函数
  await callback();
  co_return 0;
}

// 回调函数示例
void MyCallback() {
  std::cout << "Callback executed!" << std::endl;
}

Task<int> Run(){
//  await PerformCallback(MyCallback);
// co_return;
}

int main() {
  // 将回调函数传递给另一个函数
  Run();
  return 0;
}