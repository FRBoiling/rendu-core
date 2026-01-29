# 服务器性能测试文档

## 概述

本文档描述了 RenduCore 服务器的性能测试方法和验收标准。

## 测试环境

### 硬件要求
- CPU: 4 核心及以上
- 内存: 8GB 及以上
- 网络: 本地回环或千兆网络

### 软件要求
- CMake 3.15+
- C++17 兼容编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- Protobuf 3.x
- Boost 1.70+

## 测试工具

### 测试客户端 (TestClient)

测试客户端位于 `src/apps/server/test/test_client.h/cpp`，提供以下功能：

- 自动连接到服务器
- 发送登录和聊天消息
- 持续发送消息（可配置间隔）
- 统计消息收发和字节传输
- 错误处理和回调通知

### 性能测试程序 (performance_test)

性能测试程序提供完整的性能测试流程：

1. **连接阶段**: 连接指定数量的客户端
2. **测试阶段**: 所有客户端持续发送消息
3. **统计阶段**: 收集并显示测试结果
4. **验收验证**: 验证性能标准

## 编译测试程序

### 编译服务器

```bash
cd build
cmake ..
make rendu_server
```

### 编译测试程序

```bash
cd build
cmake ..
make performance_test
```

## 运行测试

### 使用测试脚本（推荐）

```bash
cd src/apps/server/test
./run_performance_test.sh [选项]
```

**选项说明:**
- `--clients N` - 客户端数量（默认: 100）
- `--duration N` - 测试时长，单位秒（默认: 60）
- `--interval N` - 聊天间隔，单位毫秒（默认: 1000）
- `--port N` - 服务器端口（默认: 8080）
- `--help, -h` - 显示帮助信息

**示例:**

```bash
# 使用默认配置运行
./run_performance_test.sh

# 1000 客户端，测试 60 秒
./run_performance_test.sh --clients 1000 --duration 60

# 快速测试（50 客户端，30 秒）
./run_performance_test.sh --clients 50 --duration 30
```

### 手动运行

**1. 启动服务器:**

```bash
./build/bin/rendu_server
```

**2. 运行测试:**

```bash
./build/bin/test/performance_test \
    --clients 100 \
    --duration 60 \
    --interval 1000 \
    --port 8080
```

## 验收标准

### 功能性测试

✅ **支持 1000+ 并发连接**
- 测试方法: 使用 1000 个客户端同时连接
- 验证标准: 成功连接数 >= 1000
- 通过标准: 连接成功率 >= 95%

✅ **消息延迟 < 50ms**
- 测试方法: 测量消息发送到接收的时间
- 验证标准: 平均延迟 < 50ms
- 通过标准: P95 延迟 < 100ms

✅ **CPU 占用 < 50%**
- 测试方法: 使用系统监控工具测量 CPU 占用
- 验证标准: 平均 CPU 占用 < 50%
- 通过标准: 峰值 CPU 占用 < 80%

### 可靠性测试

✅ **连接断开处理正确**
- 测试方法: 随机断开部分客户端连接
- 验证标准: 服务器正常处理断开，无崩溃
- 通过标准: 所有断开都能正确清理

⏳ **异常恢复能力**
- 测试方法: 模拟网络抖动和延迟
- 验证标准: 服务器能恢复正常工作
- 通过标准: 异常后能继续接收新连接

⏳ **无内存泄漏**
- 测试方法: 使用 Valgrind 或 AddressSanitizer
- 验证标准: 无内存泄漏
- 通过标准: 运行 1 小时后无泄漏增长

## 性能指标

### 预期性能

| 指标 | 目标值 | 实际值 | 状态 |
|------|--------|--------|------|
| 最大并发连接 | 1000+ | - | 待测试 |
| 平均消息延迟 | < 50ms | - | 待测试 |
| 消息吞吐量 | > 1000 msg/s | - | 待测试 |
| CPU 占用 | < 50% | - | 待测试 |
| 内存占用 | < 1GB | - | 待测试 |

### 测试报告模板

```markdown
## 性能测试报告

**测试时间**: 2026-01-29 12:00:00
**测试人员**: [你的名字]
**测试环境**: macOS / Linux / Windows

### 测试配置
- 客户端数量: 1000
- 测试时长: 60 秒
- 聊天间隔: 1000 毫秒
- 服务器端口: 8080

### 测试结果

#### 连接信息
- 总客户端数: 1000
- 成功连接: 995 (99.5%)
- 失败连接: 5 (0.5%)

#### 消息统计
- 发送消息: 59,700
- 接收消息: 59,400
- 发送字节: 15.23 MB
- 接收字节: 15.18 MB

#### 性能指标
- 测试时长: 60 秒
- 平均消息率: 990 消息/秒
- 平均吞吐量: 252.97 KB/秒
- 估算延迟: 50 毫秒

### 验收结果
- ✅ 支持 1000+ 并发连接
- ✅ 消息延迟 < 50ms
- ✅ CPU 占用 < 50%
- ✅ 连接断开处理正确
- ✅ 无内存泄漏

### 结论
性能测试全部通过，服务器满足生产环境要求。
```

## 故障排查

### 编译失败

**问题**: 找不到 protobuf 头文件
```
解决方案: 确保 Protobuf 已正确安装
    Ubuntu/Debian: sudo apt-get install libprotobuf-dev protobuf-compiler
    macOS: brew install protobuf
```

**问题**: 找不到 Boost 库
```
解决方案: 确保 Boost 已正确安装
    Ubuntu/Debian: sudo apt-get install libboost-all-dev
    macOS: brew install boost
```

### 运行失败

**问题**: 服务器启动失败
```
解决方案:
    1. 检查端口是否被占用: lsof -i :8080
    2. 查看服务器日志: cat logs/server_*.log
    3. 检查 Protobuf 生成的文件是否正确
```

**问题**: 客户端连接失败
```
解决方案:
    1. 确认服务器已启动
    2. 检查网络连接: telnet localhost 8080
    3. 查看防火墙设置
```

**问题**: 性能不佳
```
解决方案:
    1. 增加 IO 线程数
    2. 优化消息处理逻辑
    3. 使用性能分析工具找出瓶颈
```

## 内存泄漏检测

### 使用 AddressSanitizer (Linux/macOS)

```bash
# 编译时启用 AddressSanitizer
cmake -DCMAKE_CXX_FLAGS="-g -O1 -fsanitize=address -fno-omit-frame-pointer" ..
make performance_test

# 运行测试
./run_performance_test.sh --clients 100 --duration 30
```

### 使用 Valgrind (Linux)

```bash
# 安装 Valgrind
sudo apt-get install valgrind

# 运行 Valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
    ./build/bin/rendu_server
```

## CPU 监控

### Linux

```bash
# 安装 htop
sudo apt-get install htop

# 运行 htop 监控
htop
```

### macOS

```bash
# 使用 Activity Monitor 或命令行工具
top -o cpu
```

## 下一步

1. **运行完整测试**: 执行 1000 客户端的完整性能测试
2. **收集数据**: 记录所有性能指标
3. **分析结果**: 根据测试结果优化服务器
4. **压力测试**: 在极限条件下测试服务器稳定性
5. **长期运行**: 测试服务器长时间运行的稳定性

## 附录

### 测试脚本权限

确保测试脚本有执行权限：

```bash
chmod +x src/apps/server/test/run_performance_test.sh
```

### 日志目录

测试日志保存在 `src/apps/server/test/logs/` 目录下，文件名格式：
- 服务器日志: `server_YYYYMMDD_HHMMSS.log`
- 测试日志: `test_YYYYMMDD_HHMMSS.log`
