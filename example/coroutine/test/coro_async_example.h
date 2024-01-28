#include <coroutine>
#include <queue>

struct promise_type;

using HandleType = std::coroutine_handle<promise_type>;

std::queue<HandleType> Tasks;

struct promise_type
{
  auto initial_suspend() { return std::suspend_always{}; }
  auto final_suspend() noexcept { return std::suspend_never{}; }
  void return_void() noexcept {}
  void unhandled_exception() { std::rethrow_exception(std::current_exception()); }
  HandleType get_return_object() noexcept { return HandleType::from_promise(*this); }
};

struct RDTask
{
  HandleType coro_handle;
  RDTask(HandleType handle): coro_handle(handle) {}
  ~RDTask() { if (coro_handle) coro_handle.destroy(); }
  void operator()() { resume(); }
  void resume() { if (coro_handle) coro_handle.resume(); }
};

RDTask foo()
{
  while (true)
  {
    printf("Hello coroutine!\n");
    co_await std::suspend_always{};
  }
}

int coro_async_main()
{
  auto t1 = foo();
  auto t2 = foo();
  Tasks.push(t1.coro_handle);
  Tasks.push(t2.coro_handle);
  while (!Tasks.empty())
  {
    auto task = Tasks.front();
    Tasks.pop();
    task.resume();
    Tasks.push(task.coro_handle);
  }
  return 0;
}