# 阶段 3: Common 层 - 日志系统 (log)

## 目标
实现基于 io_context 的异步日志系统，提供多种输出目标和格式化能力。

---

## 任务清单

### 1. logger.h
**文件路径**: `src/common/include/common/log/logger.h`

**职责**:
- 提供统一的日志接口
- 支持多种日志级别
- 异步写入不阻塞主线程

**核心接口**:
```cpp
class Logger {
public:
    enum class Level {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    // 初始化日志系统
    static void Initialize(const std::string& name);

    // 设置日志级别
    static void SetLevel(Level level);

    // 添加 Sink
    static void AddSink(std::shared_ptr<ISink> sink);

    // 移除 Sink
    static void RemoveSink(const std::string& name);

    // 日志输出
    template<typename... Args>
    static void Trace(fmt::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    static void Debug(fmt::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    static void Info(fmt::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    static void Warn(fmt::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    static void Error(fmt::format_string<Args...> fmt, Args&&... args);

    template<typename... Args>
    static void Critical(fmt::format_string<Args...> fmt, Args&&... args);
};
```

---

### 2. sink.h
**文件路径**: `src/common/include/common/log/sink.h`

**职责**:
- Sink 抽象基类
- 定义输出目标接口

**核心接口**:
```cpp
class ISink {
public:
    virtual ~ISink() = default;

    // 设置日志级别
    virtual void SetLevel(Logger::Level level);

    // 日志写入
    virtual void Log(const LogMessage& msg) = 0;

    // 刷新缓冲区
    virtual void Flush() = 0;

    // 获取 Sink 名称
    virtual std::string GetName() const = 0;
};
```

---

### 3. console_sink.h
**文件路径**: `src/common/include/common/log/console_sink.h`

**职责**:
- 控制台输出
- 支持颜色
- 支持 stdout/stderr

**核心接口**:
```cpp
class ConsoleSink : public ISink {
public:
    enum class Target {
        StdOut,
        StdErr
    };

    ConsoleSink(Target target = Target::StdOut);

    void SetColorEnabled(bool enabled);
};
```

---

### 4. file_sink.h
**文件路径**: `src/common/include/common/log/file_sink.h`

**职责**:
- 文件输出
- 支持日志轮转
- 支持异步写入

**核心接口**:
```cpp
class FileSink : public ISink {
public:
    // 构造函数
    FileSink(const std::string& filename);

    // 设置轮转策略
    void SetRotation(size_t max_size, int max_files);

    // 强制刷新
    void Flush() override;
};
```

---

### 5. formatter.h
**文件路径**: `src/common/include/common/log/formatter.h`

**职责**:
- 日志格式化
- 支持自定义格式字符串

**核心接口**:
```cpp
class Formatter {
public:
    // 默认格式: [时间] [级别] [线程] 消息
    static std::string Format(const LogMessage& msg);

    // 自定义格式
    static void SetPattern(const std::string& pattern);
};
```

**支持的格式占位符**:
- `%T` - 时间
- `%L` - 日志级别
- `%t` - 线程 ID
- `%v` - 消息
- `%f` - 文件名
- `%l` - 行号
- `%n` - 换行

---

## 设计要点

### 1. 异步模型
- 基于 io_context 的异步队列
- 日志任务投递到 io_context
- 后台线程负责实际写入

### 2. 线程安全
- 日志接口可跨线程调用
- 内部使用 strand 保证顺序
- 无锁队列优化性能

### 3. 性能优化
- 使用 string_view 避免拷贝
- 批量写入减少 I/O
- 缓冲区管理

### 4. 零拷贝
- LogMessage 使用 shared_ptr 引用计数
- 多 Sink 共享同一消息对象

---

## 验收标准

- [ ] 日志不阻塞主线程（耗时 < 10μs）
- [ ] 多线程安全
- [ ] 文件轮转正常工作
- [ ] 格式化输出正确
- [ ] 颜色输出正常
- [ ] 性能测试通过（1M logs/s）
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 2 (io) - io_context 异步模型
- 阶段 1 (util) - 基础工具
- 3rdparty::fmt - 格式化
- 3rdparty::spdlog - 可选（作为备用实现）
