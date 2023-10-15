#include <iostream>
#include <functional>
#include <task/task.h>
#include <task/sync_wait.h>


template<typename Func, typename... Args>
auto bind(Func&& func, Args&&... args) {
  return [func = std::forward<Func>(func), ...args = std::forward<Args>(args)]() mutable {
    return func(std::forward<Args>(args)...);
  };
}

using namespace rendu;

std::vector<Task<>*> ttt;

template<typename Func>
Task<> getTask(Func func) {
  auto task = func();
  ttt.push_back(&task);
  return task;
}

void bind_task(){
//  auto makeTask = [&]() -> Task<> {
//    int result = 1+2;
//    co_return ;
//  };
  Task<> task = getTask([&]() -> Task<> {
    int result = 1+2;
    co_return ;
  });

  ttt.push_back(&task);
  sync_wait(task);
}



int main() {
  int x = 5;
  auto boundLambda = bind(
      [x](int y) {
    std::cout << "x + y = " << x + y << std::endl;
  }, 10);
  boundLambda();
  bind_task();
  return 0;
}
