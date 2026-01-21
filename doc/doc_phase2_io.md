# 阶段 2: Common 层 - I/O 抽象 (io)

## 目标
- 封装 Boost.Asio io_context，提供统一事件循环
- 实现基于 io_context 的任务调度器
- 提供定时器封装（一次性、周期性）

---

## 文件结构

```
src/common/
├── include/common/io/
│   ├── io_context.h
│   ├── scheduler.h
│   └── timer.h
└── src/io/
    ├── io_context.cpp
    ├── scheduler.cpp
    └── timer.cpp
```

---

## 任务清单

### 1. io_context 封装 (io_context.h/cpp)
- [ ] 封装 `boost::asio::io_context`
- [ ] 提供线程池支持
- [ ] 支持跨线程 post 任务
- [ ] 提供 stop/run/stop_after 接口
- [ ] RAII 管理生命周期

### 2. 任务调度器 (scheduler.h/cpp)
- [ ] 异步任务提交（post）
- [ ] 延迟任务调度（delayed）
- [ ] 周期性任务调度（periodic）
- [ ] 任务取消功能
- [ ] 任务优先级（可选）

### 3. 定时器 (timer.h/cpp)
- [ ] 一次性定时器
- [ ] 周期性定时器
- [ ] 定时器取消
- [ ] 定时器状态查询

---

## 头文件设计

### io_context.h
```cpp
#pragma once

#include <memory>
#include <functional>
#include <thread>
#include <vector>
#include <boost/asio/io_context.hpp>

namespace rendu::io {

class IoContext {
public:
    explicit IoContext(size_t thread_count = 1);
    ~IoContext();

    // 禁止拷贝和移动
    IoContext(const IoContext&) = delete;
    IoContext& operator=(const IoContext&) = delete;
    IoContext(IoContext&&) = delete;
    IoContext& operator=(IoContext&&) = delete;

    // 提交任务
    void post(std::function<void()> task);

    // 运行/停止
    void run();
    void stop();
    void stop_after(std::chrono::milliseconds timeout);

    // 获取底层 io_context（供内部使用）
    boost::asio::io_context& native();
    const boost::asio::io_context& native() const;

    // 检查是否运行中
    bool running() const;

private:
    boost::asio::io_context io_context_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_;
    std::vector<std::thread> threads_;
    bool running_;
};

} // namespace rendu::io
```

### scheduler.h
```cpp
#pragma once

#include <functional>
#include <memory>
#include <chrono>
#include <common/io/io_context.h>

namespace rendu::io {

class Scheduler {
public:
    explicit Scheduler(IoContext& io);
    ~Scheduler() = default;

    // 提交立即执行的任务
    void post(std::function<void()> task);

    // 延迟执行
    void delayed(std::function<void()> task, std::chrono::milliseconds delay);

    // 周期性执行（返回取消句柄）
    using CancelToken = std::shared_ptr<bool>;
    CancelToken periodic(std::function<void()> task, std::chrono::milliseconds interval);

    // 取消任务
    void cancel(CancelToken token);

private:
    IoContext& io_;
};

} // namespace rendu::io
```

### timer.h
```cpp
#pragma once

#include <functional>
#include <memory>
#include <chrono>
#include <boost/asio/steady_timer.hpp>
#include <common/io/io_context.h>

namespace rendu::io {

class Timer {
public:
    using Callback = std::function<void()>;

    explicit Timer(IoContext& io);
    ~Timer();

    // 禁止拷贝
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = default;
    Timer& operator=(Timer&&) = default;

    // 一次性定时器
    void expires_after(std::chrono::milliseconds delay, Callback callback);
    void expires_at(std::chrono::steady_clock::time_point time, Callback callback);

    // 周期性定时器（返回取消句柄）
    using CancelToken = std::shared_ptr<bool>;
    CancelToken repeat(std::chrono::milliseconds interval, Callback callback);

    // 取消定时器
    void cancel();

    // 检查是否激活
    bool active() const;

private:
    IoContext& io_;
    std::unique_ptr<boost::asio::steady_timer> timer_;
    CancelToken cancel_token_;
    bool active_;
};

} // namespace rendu::io
```

---

## 单元测试

### 测试文件
```
src/tests/common/io/
├── CMakeLists.txt
├── io_context_test.cpp
├── scheduler_test.cpp
└── timer_test.cpp
```

### io_context_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <common/io/io_context.h>
#include <thread>
#include <atomic>

using namespace rendu::io;

TEST_CASE("IoContext post", "[io][io_context]") {
    IoContext io(2);
    std::atomic<int> count{0};

    io.post([&count]() { count++; });
    io.post([&count]() { count++; });
    io.post([&count]() { count++; });

    std::thread([&io]() {
        io.run();
    }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();

    REQUIRE(count == 3);
}

TEST_CASE("IoContext multi-thread", "[io][io_context]") {
    IoContext io(4);
    std::atomic<int> count{0};

    for (int i = 0; i < 100; ++i) {
        io.post([&count]() { count++; });
    }

    std::thread([&io]() {
        io.run();
    }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    io.stop();

    REQUIRE(count == 100);
}
```

### scheduler_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/io/scheduler.h>
#include <atomic>
#include <thread>

using namespace rendu::io;

TEST_CASE("Scheduler delayed", "[io][scheduler]") {
    IoContext io(1);
    Scheduler scheduler(io);

    std::atomic<bool> executed{false};
    scheduler.delayed([&executed]() { executed = true; }, std::chrono::milliseconds(50));

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == false);

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == true);

    io.stop();
}

TEST_CASE("Scheduler periodic", "[io][scheduler]") {
    IoContext io(1);
    Scheduler scheduler(io);

    std::atomic<int> count{0};
    auto token = scheduler.periodic([&count]() { count++; }, std::chrono::milliseconds(20));

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(70));
    REQUIRE(count >= 3);

    scheduler.cancel(token);
    io.stop();
}
```

### timer_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/io/timer.h>
#include <atomic>
#include <thread>

using namespace rendu::io;

TEST_CASE("Timer one-shot", "[io][timer]") {
    IoContext io(1);
    Timer timer(io);

    std::atomic<bool> executed{false};
    timer.expires_after(std::chrono::milliseconds(50), [&executed]() { executed = true; });

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == false);

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == true);

    io.stop();
}

TEST_CASE("Timer repeat", "[io][timer]") {
    IoContext io(1);
    Timer timer(io);

    std::atomic<int> count{0};
    timer.repeat(std::chrono::milliseconds(20), [&count]() { count++; });

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(70));
    REQUIRE(count >= 3);

    timer.cancel();
    io.stop();
}
```

---

## 验收标准

### 功能
- [ ] io_context 可跨线程 post 任务
- [ ] 定时器精度误差 < 5ms
- [ ] 异步任务调度正确执行
- [ ] 周期性定时器可正常取消
- [ ] 无任务泄漏

### 性能
- [ ] 任务提交延迟 < 1ms
- [ ] 支持 10k+ 并发任务
- [ ] 线程池扩展正确

### 线程安全
- [ ] 多线程 post 无数据竞争
- [ ] 定时器取消无死锁

---

## 下一步
完成本阶段后，进入 **阶段 3: Common 层 - 日志系统 (log)**
