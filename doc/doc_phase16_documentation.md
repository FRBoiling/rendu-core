# 阶段 16: 文档完善

**状态**: ⏳ 未开始
**优先级**: P1
**预计工期**: 2-3 天
**开始日期**: 2026-02-06
**完成日期**: 待定

---

## 一、目标

完善文档体系,提升易用性,帮助开发者快速上手和深入了解框架。

---

## 二、任务清单

### 2.1 用户指南 (User Guide)

**创建文件**: `doc/USER_GUIDE.md`

**任务**:
- [ ] 创建用户指南文档
- [ ] 编写快速入门教程
- [ ] 编写常见问题解答 (FAQ)
- [ ] 编写最佳实践指南

#### 快速入门教程

**内容结构**:
```markdown
# RenduCore 快速入门

## 1. 环境准备

### 前置要求
- CMake >= 3.24
- C++20 兼容编译器 (GCC 9+ / Clang 12+ / MSVC 19+)

### 安装依赖
```bash
# macOS
brew install cmake boost protobuf

# Ubuntu/Debian
sudo apt install cmake libboost-all-dev libprotobuf-dev
```

## 2. 构建项目

```bash
git clone https://github.com/xxx/rendu-core.git
cd rendu-core
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
```

## 3. 运行示例

```bash
# 运行日志示例
./cmake-build-debug/src/examples/log_example

# 运行网络示例
./cmake-build-debug/src/examples/net_example
```

## 4. 运行测试

```bash
# 运行所有测试
ctest --test-dir cmake-build-debug

# 运行特定测试
./cmake-build-debug/src/tests/common/util/string_test
```

## 5. 第一个程序

创建一个简单的日志程序:

```cpp
#include "common/log/logger.h"

int main() {
    // 初始化日志系统
    Logger::instance().set_level(LogLevel::Info);

    // 记录日志
    LOG_INFO("Hello, RenduCore!");

    return 0;
}
```

编译并运行:

```bash
g++ -std=c++20 -I/path/to/rendu-core/include main.cpp \
    -L/path/to/rendu-core/lib -lcommon -lfmt -o myapp
./myapp
```

## 6. 下一步

- 阅读 [API 文档](API.md)
- 查看 [示例程序](../src/examples/)
- 了解 [最佳实践](#最佳实践)
```

#### 常见问题解答 (FAQ)

**问题列表**:

1. **Q: 如何配置日志级别?**
   ```cpp
   Logger::instance().set_level(LogLevel::Debug);
   ```

2. **Q: 如何使用自定义日志格式?**
   ```cpp
   Formatter formatter("[%Y-%m-%d %H:%M:%S] [%l] %v");
   Logger::instance().set_formatter(formatter);
   ```

3. **Q: 如何处理网络连接错误?**
   ```cpp
   socket.connect_async("127.0.0.1", 8080,
       [](const std::error_code& ec) {
           if (ec) {
               LOG_ERROR("Connection failed: {}", ec.message());
           }
       });
   ```

4. **Q: Actor 如何处理超时?**
   ```cpp
   auto future = actor_ref.ask<Request, Response>(request);
   auto response = future.wait_for(std::chrono::seconds(5));
   ```

5. **Q: 如何调试序列化问题?**
   ```cpp
   auto result = serializer.serialize(data);
   if (!result) {
       LOG_ERROR("Serialization failed: {}", result.error());
   }
   ```

6. **Q: ECS 系统如何优化性能?**
   - 使用视图而非直接遍历
   - 批量处理组件
   - 避免频繁的组件添加/移除

#### 最佳实践指南

**内容结构**:
```markdown
# RenduCore 最佳实践

## 1. 日志使用

### 推荐做法
- 使用合适的日志级别
- 记录关键路径和错误
- 使用结构化日志格式

### 避免做法
- 不要在热路径中记录大量日志
- 不要记录敏感信息
- 不要在循环中重复日志

## 2. 网络编程

### 推荐做法
- 使用异步 I/O
- 设置合理的超时
- 处理连接断开和重连

### 避免做法
- 不要阻塞 I/O 线程
- 不要忽略错误处理
- 不要创建过多连接

## 3. Actor 系统

### 推荐做法
- 保持 Actor 简单专注
- 使用不可变消息
- 避免共享状态

### 避免做法
- 不要在 Actor 中阻塞
- 不要共享可变状态
- 不要创建过多 Actor

## 4. 性能优化

### 内存管理
- 使用对象池减少分配
- 避免不必要的拷贝
- 使用移动语义

### 并发编程
- 使用 Actor 模型隔离线程
- 使用事件总线解耦
- 避免锁竞争

## 5. 错误处理

### 推荐做法
- 使用 Result 类型返回错误
- 记录错误上下文
- 提供恢复机制

### 避免做法
- 不要吞掉错误
- 不要使用异常处理控制流
- 不要忽略返回值
```

---

### 2.2 API 文档

**任务**:
- [ ] 配置 Doxygen
- [ ] 补充缺失的函数注释
- [ ] 生成完整 API 文档
- [ ] 发布到 GitHub Pages (可选)

#### Doxygen 配置

**创建文件**: `Doxyfile`

```cmake
# Doxygen 配置
PROJECT_NAME           = "RenduCore"
PROJECT_NUMBER         = "0.2.0"
OUTPUT_DIRECTORY       = "docs/api"
INPUT                  = src/common/include src/core/include
RECURSIVE              = YES
EXCLUDE_PATTERNS       = */3rdparty/* */build/*
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
HAVE_DOT               = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
```

**添加到 CMakeLists.txt**:
```cmake
option(BUILD_DOCS "Build documentation" ON)

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    else()
        message(WARNING "Doxygen not found, skipping documentation generation")
    endif()
endif()
```

#### 代码注释规范

**类注释示例**:
```cpp
/**
 * @file logger.h
 * @brief 日志系统核心类
 *
 * 提供多级别日志记录功能,支持多种输出目标和自定义格式化器。
 * 基于异步 I/O 实现,不阻塞主线程。
 */

namespace rendu {
namespace log {

/**
 * @brief 日志记录器类
 *
 * 提供统一的日志记录接口,支持多级别日志、多输出目标和自定义格式。
 * 使用单例模式,全局唯一实例。
 *
 * @example
 * Logger::instance().set_level(LogLevel::Info);
 * LOG_INFO("Server started on port {}", port);
 */
class Logger {
public:
    /**
     * @brief 获取 Logger 单例实例
     * @return Logger& 单例引用
     */
    static Logger& instance();

    /**
     * @brief 设置日志级别
     * @param level 最低输出级别
     */
    void set_level(LogLevel level);

    /**
     * @brief 添加输出目标
     * @param sink Sink 指针,生命周期由调用者管理
     */
    void add_sink(Sink* sink);
};
```

#### 需要补充注释的文件

**Common 层**:
- [ ] `common/io/io_context.h`
- [ ] `common/io/scheduler.h`
- [ ] `common/io/timer.h`
- [ ] `common/net/socket.h`
- [ ] `common/net/channel.h`
- [ ] `common/net/codec.h`
- [ ] `common/ser/serializer.h`
- [ ] `common/ser/protobuf_ser.h`
- [ ] `common/ser/json_ser.h`
- [ ] `common/event/event.h`
- [ ] `common/event/event_bus.h`
- [ ] `common/event/handler.h`
- [ ] `common/config/config.h`
- [ ] `common/config/loader.h`
- [ ] `common/ecs/registry.h`
- [ ] `common/ecs/system.h`
- [ ] `common/ecs/view.h`

**Core 层**:
- [ ] `core/engine/engine.h`
- [ ] `core/engine/context.h`
- [ ] `core/loop/loop.h`
- [ ] `core/loop/fixed_loop.h`
- [ ] `core/state/state.h`
- [ ] `core/state/state_machine.h`
- [ ] `core/lifecycle/lifecycle.h`
- [ ] `core/lifecycle/lifecycle_manager.h`
- [ ] `core/actor/actor.h`
- [ ] `core/actor/actor_system.h`
- [ ] `core/actor/actor_ref.h`
- [ ] `core/actor/message.h`
- [ ] `core/actor/message_queue.h`

---

### 2.3 性能优化指南

**创建文件**: `doc/PERFORMANCE_GUIDE.md`

**内容结构**:
```markdown
# RenduCore 性能优化指南

## 1. 内存优化

### 1.1 对象池
使用对象池减少小对象分配:

```cpp
template<typename T>
class ObjectPool {
public:
    std::shared_ptr<T> acquire();
    void release(std::shared_ptr<T> obj);
};
```

### 1.2 零拷贝优化
使用视图而非拷贝:

```cpp
// 推荐: 使用 string_view
void process_data(std::string_view data);

// 避免: 拷贝字符串
void process_data(const std::string& data);
```

### 1.3 移动语义
使用移动语义减少拷贝:

```cpp
// 推荐: 使用 std::move
std::vector<Message> messages;
send_messages(std::move(messages));

// 避免: 拷贝
send_messages(messages);
```

## 2. 网络性能

### 2.1 连接复用
- 使用连接池复用连接
- 避免频繁创建/销毁连接

### 2.2 批量处理
- 批量发送消息
- 减少系统调用次数

### 2.3 TCP 参数调优
```cpp
// 禁用 Nagle 算法 (低延迟场景)
socket.set_option(tcp::no_delay(true));

// 设置接收缓冲区
socket.set_option(socket_base::receive_buffer_size(65536));

// 设置发送缓冲区
socket.set_option(socket_base::send_buffer_size(65536));
```

## 3. 多线程性能

### 3.1 Actor 池
使用 Actor 池复用 Actor:

```cpp
// 推荐: 使用 Actor 池
auto actor = actor_pool.acquire();
actor.process_message(message);
actor_pool.release(actor);

// 避免: 每次创建新 Actor
auto actor = actor_system.create<WorkerActor>();
```

### 3.2 避免锁竞争
- 使用 Actor 模型隔离状态
- 使用无锁数据结构
- 减少临界区范围

### 3.3 CPU 亲和性
绑定线程到特定 CPU 核心:

```cpp
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(core_id, &cpuset);
pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
```

## 4. 日志性能

### 4.1 异步日志
使用异步日志不阻塞主线程:

```cpp
// 推荐: 使用 FileSink (默认异步)
FileSink file_sink("app.log");
Logger::instance().add_sink(&file_sink);

// 避免: 同步日志会阻塞
// FileSink 支持配置同步模式
```

### 4.2 日志级别
设置合适的日志级别:

```cpp
// 生产环境: 只记录 Error 和 Critical
Logger::instance().set_level(LogLevel::Error);

// 开发环境: 记录所有级别
Logger::instance().set_level(LogLevel::Trace);
```

### 4.3 格式化优化
使用预编译格式化器:

```cpp
// 推荐: 使用编译期格式化
LOG_INFO("Value: {}, Count: {}", value, count);

// 避免: 运行时拼接字符串
LOG_INFO("Value: " + std::to_string(value));
```

## 5. ECS 性能

### 5.1 视图查询
使用视图优化查询:

```cpp
// 推荐: 使用视图
auto view = registry.view<Position, Velocity>();
for (auto entity : view) {
    // 批量处理
}

// 避免: 逐个查询
for (auto entity : entities) {
    if (registry.has<Position>(entity) &&
        registry.has<Velocity>(entity)) {
        // 处理
    }
}
```

### 5.2 组件布局
- 相邻组件缓存友好
- 使用紧凑数据结构
- 避免虚函数

## 6. 性能基准测试

### 6.1 常用性能指标

| 指标 | 目标值 | 测量方法 |
|------|--------|---------|
| 消息吞吐量 | >10000 msg/s | 性能测试工具 |
| 消息延迟 | <20ms (P99) | 百分位统计 |
| 内存使用 | <500MB (空闲) | 系统监控 |
| CPU 使用率 | <80% (满载) | top/htop |

### 6.2 性能分析工具
- **Valgrind**: 内存分析
- **perf**: CPU 性能分析
- **gprof**: 函数调用统计
- **heaptrack**: 堆分配跟踪
```

---

### 2.4 部署指南

**创建文件**: `doc/DEPLOYMENT_GUIDE.md`

**内容结构**:
```markdown
# RenduCore 部署指南

## 1. 生产环境部署

### 1.1 编译 Release 版本
```bash
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release -j$(nproc)
```

### 1.2 打包
```bash
# 创建安装包
cmake --install cmake-build-release --prefix /opt/rendu-core

# 或创建 DEB 包
cpack -G DEB
```

### 1.3 系统服务配置

创建 systemd 服务文件 `/etc/systemd/system/rendu-server.service`:

```ini
[Unit]
Description=RenduCore Server
After=network.target

[Service]
Type=simple
User=rendu
Group=rendu
WorkingDirectory=/opt/rendu-core/bin
ExecStart=/opt/rendu-core/bin/server --config /etc/rendu/config.json
Restart=on-failure
RestartSec=10

# 资源限制
LimitNOFILE=65536
LimitNPROC=4096

[Install]
WantedBy=multi-user.target
```

启动服务:
```bash
sudo systemctl daemon-reload
sudo systemctl enable rendu-server
sudo systemctl start rendu-server
```

## 2. 监控配置

### 2.1 日志配置

**config.json**:
```json
{
  "logging": {
    "level": "INFO",
    "sinks": {
      "console": {
        "enabled": true,
        "level": "INFO"
      },
      "file": {
        "enabled": true,
        "path": "/var/log/rendu/app.log",
        "level": "INFO",
        "rotation": "daily",
        "max_size": 100
      }
    }
  }
}
```

### 2.2 性能监控

集成 Prometheus:

```cpp
#include <prometheus/registry.h>

// 创建注册表
auto registry = std::make_shared<prometheus::Registry>();

// 创建指标
auto& gauge = prometheus::build::gauge()
    .name("active_connections")
    .register(*registry);

// 更新指标
gauge.set(connection_count);

// 启动 HTTP 端口暴露指标
auto exposer = prometheus::build_exposer(":9090").register(*registry);
```

查询指标:
```bash
curl http://localhost:9090/metrics
```

### 2.3 健康检查

实现健康检查端点:

```cpp
void handle_health_check(HttpRequest& req, HttpResponse& res) {
    HealthStatus status;
    status.database = check_database();
    status.memory = check_memory();
    status.disk = check_disk();

    if (status.healthy()) {
        res.set_status(HttpStatusCode::OK);
        res.set_body(to_json(status));
    } else {
        res.set_status(HttpStatusCode::ServiceUnavailable);
        res.set_body(to_json(status));
    }
}
```

## 3. 故障排查

### 3.1 常见问题

**问题**: 服务无法启动
**排查步骤**:
1. 检查端口占用: `netstat -tulpn | grep 8080`
2. 检查配置文件语法
3. 查看日志: `journalctl -u rendu-server -f`

**问题**: 连接数达到上限
**解决方案**:
```bash
# 增加文件描述符限制
ulimit -n 65536

# 永久生效 (编辑 /etc/security/limits.conf)
rendu soft nofile 65536
rendu hard nofile 65536
```

**问题**: 内存泄漏
**排查步骤**:
1. 使用 Valgrind 检测
2. 查看 /proc/{pid}/maps
3. 分析堆转储

**问题**: CPU 使用率过高
**排查步骤**:
1. 使用 perf 分析热点函数
2. 检查死循环
3. 优化锁竞争

### 3.2 日志分析

使用 ELK Stack 分析日志:
```bash
# Logstash 配置
input {
  file {
    path => "/var/log/rendu/*.log"
    codec => json
  }
}

filter {
  if [level] == "ERROR" or [level] == "CRITICAL" {
    # 发送告警
  }
}

output {
  elasticsearch {
    hosts => ["localhost:9200"]
  }
}
```

## 4. 备份和恢复

### 4.1 配置备份
```bash
# 备份配置
tar -czf config-backup-$(date +%Y%m%d).tar.gz /etc/rendu/

# 定时备份 (crontab)
0 2 * * * /usr/local/bin/backup-config.sh
```

### 4.2 数据备份
```bash
# 备份数据库
pg_dump -U rendu -d rendu_db > backup-$(date +%Y%m%d).sql

# 恢复
psql -U rendu -d rendu_db < backup-20260131.sql
```

## 5. 升级和回滚

### 5.1 滚动升级
```bash
# 1. 停止旧服务
sudo systemctl stop rendu-server

# 2. 备份旧版本
sudo cp -r /opt/rendu-core /opt/rendu-core.backup

# 3. 安装新版本
sudo dpkg -i rendu-core_0.2.0_amd64.deb

# 4. 启动新服务
sudo systemctl start rendu-server

# 5. 验证
sudo systemctl status rendu-server
```

### 5.2 回滚
```bash
# 停止新服务
sudo systemctl stop rendu-server

# 恢复旧版本
sudo rm -rf /opt/rendu-core
sudo mv /opt/rendu-core.backup /opt/rendu-core

# 启动旧服务
sudo systemctl start rendu-server
```
```

---

## 三、验收标准

- [ ] 用户指南文档完整
- [ ] API 文档覆盖所有公开接口
- [ ] 性能优化指南实用
- [ ] 部署指南可操作
- [ ] 文档与代码实现同步
- [ ] 文档格式统一清晰

---

## 四、依赖关系

- 阶段 15 (示例程序)
- 阶段 16 (技术债务清理)

---

## 五、风险评估

| 风险 | 影响 | 概率 | 应对措施 |
|------|------|------|---------|
| 文档不完整 | 中 | 中 | 参考优秀项目文档模板 |
| 代码注释遗漏 | 低 | 高 | 使用静态检查工具 |
| 示例代码失效 | 低 | 低 | 定期验证示例可运行性 |

---

## 六、进度跟踪

| 任务 | 负责人 | 状态 | 预计完成时间 |
|------|--------|------|-------------|
| 用户指南编写 | boil | ⏳ | 2026-02-07 |
| API 文档生成 | boil | ⏳ | 2026-02-07 |
| 性能优化指南 | boil | ⏳ | 2026-02-08 |
| 部署指南 | boil | ⏳ | 2026-02-08 |
| 代码注释补充 | boil | ⏳ | 2026-02-09 |

---

## 七、文档维护

### 文档更新流程

1. **代码变更**: 修改代码时同步更新相关文档
2. **文档审查**: 提交 PR 时检查文档同步性
3. **定期审查**: 每月检查文档是否过期
4. **用户反馈**: 根据用户反馈改进文档

### 文档版本管理

- 使用 Git 跟踪文档变更
- 文档版本与项目版本同步
- 重要变更记录在 CHANGELOG

---

**文档版本**: v1.0
**最后更新**: 2026-01-31
