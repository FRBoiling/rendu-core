# RenduCore 常见问题 (FAQ)

**版本**: 1.0
**更新日期**: 2026-02-01

---

## 目录

1. [编译和构建](#编译和构建)
2. [使用和配置](#使用和配置)
3. [性能优化](#性能优化)
4. [错误排查](#错误排查)
5. [高级功能](#高级功能)

---

## 编译和构建

### Q1: 编译时出现 boost 相关错误怎么办？

**A**: 确保正确安装了 boost 库：

```bash
# macOS
brew install boost

# Ubuntu/Debian
sudo apt install libboost-all-dev

# Windows (使用 vcpkg)
vcpkg install boost-asio boost-system
```

然后在 CMake 中设置 boost 路径：

```cmake
set(Boost_INCLUDE_DIR /path/to/boost/include)
set(Boost_LIBRARY_DIR /path/to/boost/lib)
```

### Q2: CMake 提示找不到依赖库？

**A**: 检查以下几点：

1. 确认依赖已正确安装
2. 设置 CMAKE_PREFIX_PATH 指向安装路径：

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/usr/local
```

3. 使用 `--debug-find-pkg` 查看查找详情：

```bash
cmake -B build --debug-find-pkg
```

### Q3: 如何生成静态库？

**A**: 在 CMake 配置时设置 `BUILD_SHARED_LIBS` 选项：

```bash
cmake -B build -DBUILD_SHARED_LIBS=OFF
```

### Q4: 如何启用编译优化？

**A**: 使用 Release 模式构建：

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## 使用和配置

### Q5: 如何配置日志级别？

**A**: 使用 `set_level()` 方法：

```cpp
auto& logger = log::default_logger();
logger.set_level(log::Level::Debug);
```

日志级别包括：
- `Level::Trace` - 最详细
- `Level::Debug` - 调试信息
- `Level::Info` - 普通信息（默认）
- `Level::Warn` - 警告
- `Level::Error` - 错误
- `Level::Critical` - 严重错误

### Q6: 如何同时输出到控制台和文件？

**A**: 添加多个 Sink：

```cpp
auto& logger = log::default_logger();
logger.add_sink(std::make_shared<log::ConsoleSink>());
logger.add_sink(std::make_shared<log::FileSink>("app.log"));
```

### Q7: 配置热更新不生效？

**A**: 检查以下几点：

1. 确保 `ConfigWatcher` 已启动：
```cpp
watcher.start();
```

2. 确保 IoContext 正在运行：
```cpp
std::thread([&io]() { io.run(); }).detach();
```

3. 检查检查间隔是否合理：
```cpp
ConfigWatcher watcher(io, "config.json", std::chrono::milliseconds(100));
```

### Q8: 如何处理 JSON 配置中的复杂类型？

**A**: 支持以下类型：
- 基础类型: `int64_t`, `double`, `bool`, `string`
- 数组类型: `vector<int64_t>`, `vector<double>`, etc.
- 映射类型: `unordered_map<string, int64_t>`, etc.
- 嵌套对象: 通过点号分隔的路径访问

```cpp
// 嵌套对象
auto db_config = config.get_sub_config("database");
auto host = db_config.get<std::string>("host");

// 或直接使用路径
auto host = config.get<std::string>("database.host");
```

### Q9: 如何使用 Protobuf 序列化？

**A**: 使用 `ProtobufSer` 类：

```cpp
#include "common/ser/protobuf_ser.h"

MyMessage msg;
msg.set_name("test");
msg.set_value(42);

// 序列化
auto data = ProtobufSer::serialize(msg);

// 反序列化
auto result = ProtobufSer::deserialize<MyMessage>(data);
if (std::holds_alternative<MyMessage>(result)) {
    auto msg = std::get<MyMessage>(result);
}
```

---

## 性能优化

### Q10: 如何提高网络性能？

**A**: 几种优化方法：

1. **使用多线程 IoContext**:
```cpp
io::IoContext io(4); // 4 个线程
```

2. **调整 TCP 参数**:
```cpp
socket.set_option(boost::asio::ip::tcp::no_delay(true));
```

3. **使用零拷贝技术**:
```cpp
channel.send(std::move(data)); // 避免拷贝
```

4. **批量处理消息**:
```cpp
// 在 Channel 中批量处理
```

### Q11: 如何减少内存分配？

**A**:
1. 使用对象池：
```cpp
// 使用 entt 的内存管理
```

2. 使用移动语义：
```cpp
channel.send(std::move(data));
```

3. 预分配缓冲区：
```cpp
std::vector<uint8_t> buffer;
buffer.reserve(4096);
```

### Q12: 如何优化日志性能？

**A**:
1. 异步日志（默认已启用）
2. 在生产环境使用 INFO 或更高级别：
```cpp
logger.set_level(log::Level::Info);
```
3. 只在需要时启用文件日志
4. 使用结构化日志：
```cpp
logger.info_fields("User action", "user_id", user_id, "action", action);
```

---

## 错误排查

### Q13: 连接被拒绝 (Connection Refused) 怎么办？

**A**: 检查以下几点：

1. 确认服务器正在运行
2. 检查端口号是否正确
3. 检查防火墙设置
4. 检查服务器绑定地址（0.0.0.0 或 127.0.0.1）

### Q14: 程序崩溃，如何获取堆栈跟踪？

**A**:
1. 在 Debug 模式下编译
2. 使用调试器运行：
```bash
lldb ./my_program
run
bt  # 查看堆栈
```
3. 检查日志中的错误信息

### Q15: Actor 消息丢失怎么办？

**A**: 检查以下几点：

1. 确认 Actor 已创建
2. 确认消息类型匹配
3. 检查 Actor 是否在处理消息时崩溃
4. 添加日志跟踪消息流转

### Q16: 配置加载失败？

**A**: 检查以下几点：

1. 确认文件路径正确
2. 检查文件格式是否正确（JSON）
3. 查看错误消息：
```cpp
auto result = loader.load("config.json");
if (std::holds_alternative<Error>(result)) {
    std::cout << "Error: " << std::get<Error>(result).message() << std::endl;
}
```

---

## 高级功能

### Q17: 如何实现自定义 Sink？

**A**: 继承 `Sink` 接口：

```cpp
#include "common/log/sink.h"

class CustomSink : public log::Sink {
public:
    void log(const LogMessage& msg) override {
        // 自定义日志处理
        std::cout << msg.message << std::endl;
    }
};

// 使用
logger.add_sink(std::make_shared<CustomSink>());
```

### Q18: 如何实现自定义事件？

**A**: 继承 `Event` 类：

```cpp
#include "common/event/event.h"

class MyEvent : public event::Event {
public:
    std::string data;

    MyEvent(const std::string& d) : data(d) {}
};

// 发布
bus.publish(MyEvent("Hello"));

// 订阅
bus.subscribe<MyEvent>([](const MyEvent& e) {
    std::cout << e.data << std::endl;
});
```

### Q19: 如何实现状态转换？

**A**: 在状态机的 update 中检查条件：

```cpp
class GameState : public State {
public:
    void update(float dt) override {
        // 检查游戏是否结束
        if (is_game_over()) {
            state_machine().change_state("game_over");
        }
    }
};
```

### Q20: 如何使用多个 EventBus？

**A**: 为不同模块创建独立的事件总线：

```cpp
event::EventBus network_bus(io);
event::EventBus game_bus(io);

// 各模块使用自己的总线
network_bus.publish<NetworkEvent>(...);
game_bus.publish<GameEvent>(...);
```

### Q21: 如何优雅地关闭服务器？

**A**:
1. 停止接受新连接
2. 通知现有连接关闭
3. 等待所有连接关闭
4. 停止 IoContext

```cpp
// 停止接受新连接
server.stop_accepting();

// 通知所有客户端
for (auto& client : clients) {
    client->send("Server shutting down...");
    client->close();
}

// 等待
std::this_thread::sleep_for(std::chrono::seconds(1));

// 停止
io.stop();
```

---

## 其他问题

### Q22: 如何贡献代码？

**A**: 参考贡献指南：

1. Fork 项目仓库
2. 创建特性分支
3. 编写代码和测试
4. 提交 Pull Request
5. 等待代码审查

### Q23: 如何报告 Bug？

**A**: 在 GitHub Issues 中报告：

1. 提供详细的问题描述
2. 提供复现步骤
3. 提供环境信息（OS、编译器、版本）
4. 提供错误日志或堆栈跟踪

### Q24: 如何获取帮助？

**A**: 几种方式：

1. 查看文档：`doc/` 目录
2. 查看示例：`src/examples/` 目录
3. 查看测试：`src/tests/` 目录
4. 在 GitHub Issues 提问
5. 联系维护者

---

## 版本历史

| 版本 | 日期 | 变更 |
|------|------|------|
| 1.0 | 2026-02-01 | 初始版本 |

---

**最后更新**: 2026-02-01
