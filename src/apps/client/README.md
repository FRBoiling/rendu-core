# 客户端测试工具

## 概述

本项目提供用于测试 RenduCore 服务器的客户端工具集，包括单个测试客户端和性能测试程序。

## 目录结构

```
client/
├── CMakeLists.txt              # 构建配置
├── README.md                  # 本文档
├── protocol/                  # 协议定义（与 server 共享）
│   └── messages.proto
├── test_client.h              # 测试客户端头文件
├── test_client.cpp             # 测试客户端实现
├── performance_test.cpp        # 性能测试程序
└── PERFORMANCE_TEST.md        # 性能测试文档
```

## 功能特性

### 测试客户端 (test_client)

单个客户端模拟器，支持以下功能：

- **自动连接** - 自动重连机制
- **登录功能** - 支持设置用户名
- **聊天功能** - 发送和接收聊天消息
- **统计收集** - 实时统计连接、消息、延迟等
- **日志输出** - 彩色日志，便于调试

#### 使用方法

```bash
# 编译
cd build
cmake ..
make rendu_test_client

# 运行
./bin/rendu_test_client --host 127.0.0.1 --port 8080 --username testuser
```

#### 命令行参数

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--host` | 服务器地址 | 127.0.0.1 |
| `--port` | 服务器端口 | 8080 |
| `--username` | 用户名 | Anonymous |
| `--interval` | 自动发送消息间隔(秒) | 5 |
| `--auto-chat` | 启用自动聊天 | false |

### 性能测试程序 (performance_test)

批量客户端管理器，用于服务器压力测试：

- **批量连接** - 支持创建多个并发客户端
- **性能监控** - 实时监控延迟、吞吐量、错误率
- **测试报告** - 生成详细的测试报告
- **自动化测试** - 支持测试脚本自动化执行

#### 使用方法

```bash
# 编译
cd build
cmake ..
make rendu_performance_test

# 运行
./bin/rendu_performance_test --clients 100 --duration 60 --messages 1000
```

#### 命令行参数

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--host` | 服务器地址 | 127.0.0.1 |
| `--port` | 服务器端口 | 8080 |
| `--clients` | 并发客户端数 | 10 |
| `--duration` | 测试持续时间(秒) | 60 |
| `--messages` | 每客户端发送消息数 | 100 |
| `--interval` | 消息发送间隔(ms) | 1000 |
| `--report` | 生成测试报告 | false |

## 性能测试指标

测试工具会收集以下性能指标：

| 指标 | 说明 | 目标值 |
|------|------|--------|
| 并发连接数 | 同时连接的客户端数量 | 1000+ |
| 消息延迟 | 消息往返时间 | < 50ms |
| 吞吐量 | 每秒处理消息数 | > 10k |
| 错误率 | 失败消息占比 | < 0.1% |

## 构建依赖

- **Boost** - 网络和系统功能
- **Protobuf** - 协议序列化
- **fmt** - 日志格式化
- **RenduCore** - 框架核心库

## 详细文档

详见 [PERFORMANCE_TEST.md](PERFORMANCE_TEST.md) 了解性能测试的详细说明。

## 示例

### 基本测试

```bash
# 启动单个客户端
./bin/rendu_test_client --host 127.0.0.1 --port 8080 --username alice

# 自动聊天模式
./bin/rendu_test_client --host 127.0.0.1 --port 8080 --username alice --auto-chat --interval 3
```

### 性能测试

```bash
# 轻量测试（10 客户端，60 秒）
./bin/rendu_performance_test --clients 10 --duration 60

# 中等测试（100 客户端，300 秒）
./bin/rendu_performance_test --clients 100 --duration 300

# 重度测试（1000 客户端，600 秒）
./bin/rendu_performance_test --clients 1000 --duration 600 --messages 500

# 生成测试报告
./bin/rendu_performance_test --clients 100 --duration 60 --report
```

## 注意事项

1. **端口占用** - 确保服务器已启动并监听指定端口
2. **系统限制** - 大规模并发测试可能需要调整系统 `ulimit` 设置
3. **网络延迟** - 测试结果受网络环境影响，建议在局域网测试
4. **资源占用** - 1000+ 并发连接测试会占用较多内存和 CPU

## 后续计划

- [ ] 添加 UI 界面
- [ ] 支持更多消息类型
- [ ] 添加测试脚本编辑器
- [ ] 支持测试配置文件

## 问题反馈

如有问题，请提交 Issue 或 Pull Request。
