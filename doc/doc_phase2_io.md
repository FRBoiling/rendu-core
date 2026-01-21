# 阶段 2: Common 层 - I/O 抽象 (io)

## 目标
建立统一的异步 I/O 模型，提供任务调度和定时器能力。

---

## 任务清单

### 1. io_context.h
**文件路径**: `src/common/include/common/io/io_context.h`

**职责**:
- 封装 Boost.Asio 的 io_context
- 提供统一的异步 I/O 接口
- 管理线程池

**核心接口**:
```cpp
class IOContext {
public:
    // 运行 io_context（阻塞当前线程）
    void Run();

    // 停止 io_context
    void Stop();

    // 提交任务到 io_context
    template<typename Task>
    void Post(Task&& task);

    // 获取底层 io_context 引用
    asio::io_context& Get();

    // 获取线程数量
    size_t GetThreadCount() const;
};
```

---

### 2. scheduler.h
**文件路径**: `src/common/include/common/io/scheduler.h`

**职责**:
- 基于io_context 的任务调度器
- 支持优先级调度
- 支持任务取消

**核心接口**:
```cpp
class Scheduler {
public:
    // 调度一次性任务
    template<typename Task>
    void Schedule(Task&& task);

    // 调度延迟任务
    template<typename Task>
    void ScheduleDelayed(Task&& task, std::chrono::milliseconds delay);

    // 调度周期性任务
    template<typename Task>
    void SchedulePeriodic(Task&& task, std::chrono::milliseconds interval);

    // 取消任务
    void Cancel(TaskId id);
};
```

---

### 3. timer.h
**文件路径**: `src/common/include/common/io/timer.h`

**职责**:
- 定时器封装
- 支持一次性定时器
- 支持周期性定时器

**核心接口**:
```cpp
class Timer {
public:
    // 设置定时器回调
    template<typename Callback>
    void SetCallback(Callback&& cb);

    // 启动一次性定时器
    void StartOnce(std::chrono::milliseconds delay);

    // 启动周期性定时器
    void StartPeriodic(std::chrono::milliseconds interval);

    // 停止定时器
    void Stop();
};
```

---

## 设计要点

### 1. 线程模型
- io_context 支持多线程运行
- 默认线程数 = CPU 核心数
- 可配置线程数

### 2. 任务调度
- 基于 io_context 的 post 机制
- 支持任务优先级（通过 strand 保证顺序）
- 异常安全

### 3. 定时器精度
- 使用高精度时钟
- 支持微秒级精度
- 处理时钟漂移

---

## 验收标准

- [ ] io_context 可多线程运行
- [ ] 任务调度正确，无死锁
- [ ] 定时器精度符合预期（误差 < 1ms）
- [ ] 周期性定时器稳定运行
- [ ] 资源正确释放
- [ ] 单元测试覆盖
- [ ] 性能测试通过

---

## 依赖
- 阶段 1 (util) - 基础工具
- 3rdparty::boost - Boost.Asio
