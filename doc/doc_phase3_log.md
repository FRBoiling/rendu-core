# 阶段 3: Common 层 - 日志系统 (log)

## 目标
- 实现基于 io_context 的异步日志系统
- 提供多种输出目标（控制台、文件、网络）
- 支持日志格式化和文件轮转

---

## 文件结构

```
src/common/
├── include/common/log/
│   ├── logger.h
│   ├── sink.h
│   ├── console_sink.h
│   ├── file_sink.h
│   └── formatter.h
└── src/log/
    ├── logger.cpp
    ├── sink.cpp
    ├── console_sink.cpp
    ├── file_sink.cpp
    └── formatter.cpp
```

---

## 任务清单

### 1. 日志接口 (logger.h/cpp)
- [ ] 日志级别：Trace/Debug/Info/Warn/Error/Critical
- [ ] 异步日志写入（基于 io_context）
- [ ] 多 Logger 支持（按模块命名）
- [ ] 全局默认 Logger
- [ ] 宏定义简化调用

### 2. Sink 抽象 (sink.h/cpp)
- [ ] Sink 抽象基类
- [ ] 日志过滤（按级别）
- [ ] 线程安全

### 3. 控制台 Sink (console_sink.h/cpp)
- [ ] 支持颜色输出
- [ ] 支持终端检测（自动关闭非终端颜色）

### 4. 文件 Sink (file_sink.h/cpp)
- [ ] 按日期轮转（daily）
- [ ] 按大小轮转（size-based）
- [ ] 异步写入（基于 io_context）
- [ ] 文件路径自动创建

### 5. 格式化器 (formatter.h/cpp)
- [ ] 默认格式：`[时间] [级别] [线程] 消息`
- [ ] 支持自定义格式
- [ ] 支持结构化字段（key-value）

---

## 头文件设计

### logger.h
```cpp
#pragma once

#include <string>
#include <memory>
#include <common/io/io_context.h>

namespace rendu::log {

enum class Level : uint8_t {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

class Sink;

class Logger {
public:
    explicit Logger(std::string name, IoContext& io);
    ~Logger();

    // 添加 Sink
    void add_sink(std::shared_ptr<Sink> sink);
    void remove_sink(std::shared_ptr<Sink> sink);
    void clear_sinks();

    // 设置日志级别
    void set_level(Level level);
    Level level() const;

    // 日志输出
    void log(Level level, const std::string& msg);

    // 便捷方法
    void trace(const std::string& msg);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);

    const std::string& name() const;

private:
    std::string name_;
    IoContext& io_;
    Level level_;
    std::vector<std::shared_ptr<Sink>> sinks_;
};

// 全局默认 Logger
Logger& default_logger();

// 获取/创建命名 Logger
Logger& get_logger(const std::string& name);
void set_default_logger(std::shared_ptr<Logger> logger);

// 宏定义
#define RENDU_LOG_TRACE(msg) RENDU_LOG(::rendu::log::Level::Trace, msg)
#define RENDU_LOG_DEBUG(msg) RENDU_LOG(::rendu::log::Level::Debug, msg)
#define RENDU_LOG_INFO(msg)  RENDU_LOG(::rendu::log::Level::Info, msg)
#define RENDU_LOG_WARN(msg)  RENDU_LOG(::rendu::log::Level::Warn, msg)
#define RENDU_LOG_ERROR(msg) RENDU_LOG(::rendu::log::Level::Error, msg)
#define RENDU_LOG_CRITICAL(msg) RENDU_LOG(::rendu::log::Level::Critical, msg)

#define RENDU_LOG(level, msg) \
    ::rendu::log::default_logger().log(level, msg)

#define RENDU_NAMED_LOG(name, level, msg) \
    ::rendu::log::get_logger(name).log(level, msg)

} // namespace rendu::log
```

### sink.h
```cpp
#pragma once

#include <string>
#include <memory>
#include <common/log/formatter.h>

namespace rendu::log {

struct LogMessage {
    Level level;
    std::string logger_name;
    std::string message;
    std::string timestamp;
    std::thread::id thread_id;
};

class Sink {
public:
    explicit Sink(Level level = Level::Info);
    virtual ~Sink() = default;

    void set_level(Level level);
    Level level() const;

    void set_formatter(std::shared_ptr<Formatter> formatter);

    void log(const LogMessage& msg);

protected:
    virtual void write(const std::string& formatted) = 0;

    bool should_log(Level level) const;

    Level level_;
    std::shared_ptr<Formatter> formatter_;
};

} // namespace rendu::log
```

### console_sink.h
```cpp
#pragma once

#include <common/log/sink.h>

namespace rendu::log {

class ConsoleSink : public Sink {
public:
    ConsoleSink(Level level = Level::Info);
    ~ConsoleSink() override = default;

protected:
    void write(const std::string& formatted) override;

private:
    bool use_color_;
    bool is_terminal_;
};

} // namespace rendu::log
```

### file_sink.h
```cpp
#pragma once

#include <common/log/sink.h>
#include <fstream>

namespace rendu::log {

class FileSink : public Sink {
public:
    enum class Rotation {
        None,
        Daily,
        Size
    };

    explicit FileSink(const std::string& filename, Level level = Level::Info);
    ~FileSink() override;

    void set_rotation(Rotation rotation, size_t max_size = 10 * 1024 * 1024); // 10MB

protected:
    void write(const std::string& formatted) override;

private:
    void rotate();
    std::string generate_filename(const std::string& base, int index);

    std::string filename_;
    Rotation rotation_;
    size_t max_size_;
    size_t current_size_;
    std::ofstream file_;
};

} // namespace rendu::log
```

### formatter.h
```cpp
#pragma once

#include <string>
#include <common/log/logger.h>

namespace rendu::log {

class Formatter {
public:
    Formatter() = default;
    virtual ~Formatter() = default;

    virtual std::string format(const LogMessage& msg) = 0;
};

class DefaultFormatter : public Formatter {
public:
    DefaultFormatter() = default;
    ~DefaultFormatter() override = default;

    std::string format(const LogMessage& msg) override;

private:
    std::string level_to_string(Level level);
};

class PatternFormatter : public Formatter {
public:
    explicit PatternFormatter(const std::string& pattern);
    ~PatternFormatter() override = default;

    std::string format(const LogMessage& msg) override;

private:
    std::string pattern_;
};

} // namespace rendu::log
```

---

## 单元测试

### 测试文件
```
src/tests/common/log/
├── CMakeLists.txt
├── logger_test.cpp
├── sink_test.cpp
├── console_sink_test.cpp
├── file_sink_test.cpp
└── formatter_test.cpp
```

### logger_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/log/logger.h>
#include <common/log/console_sink.h>
#include <common/io/io_context.h>
#include <thread>

using namespace rendu::log;
using namespace rendu::io;

TEST_CASE("Logger basic", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    logger.add_sink(std::make_shared<ConsoleSink>());

    REQUIRE_NOTHROW(logger.info("test message"));

    io.stop();
}

TEST_CASE("Logger level filtering", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    logger.set_level(Level::Warn);

    // Info 级别被过滤
    logger.info("should not appear");

    io.stop();
}
```

---

## 验收标准

### 功能
- [ ] 日志写入不阻塞主线程
- [ ] 多线程安全
- [ ] 日志级别过滤正确
- [ ] 文件轮转正常
- [ ] 控制台颜色正确显示

### 性能
- [ ] 异步日志延迟 < 100ms
- [ ] 支持 10k+ 日志/秒

### 可靠性
- [ ] 日志无丢失
- [ ] 异常恢复（文件打开失败等）

---

## 下一步
完成本阶段后，进入 **阶段 4: Common 层 - 网络通信 (net)**
