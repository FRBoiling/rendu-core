# 阶段 19: 生产就绪

**状态**: ⏳ 未开始
**优先级**: P1
**预计工期**: 3-5 天
**开始日期**: 2026-02-24
**完成日期**: 待定

---

## 一、目标

使框架适用于生产环境,确保稳定性、安全性和可维护性。

---

## 二、任务清单

### 2.1 稳定性增强

#### 2.1.1 压力测试

**测试目标**:
- 10000+ 并发连接
- 100000+ 消息/秒吞吐量
- 72小时稳定运行

**测试套件**:

**创建文件**: `src/tests/stress/stress_test.cpp`

```cpp
#include <thread>
#include <vector>
#include <atomic>
#include "core/actor/actor_system.h"
#include "common/net/socket.h"

class StressTest {
public:
    struct Config {
        size_t num_connections = 10000;
        size_t messages_per_connection = 1000;
        std::chrono::seconds test_duration{60};
    };

    void run(const Config& config) {
        std::vector<std::thread> threads;
        std::atomic<size_t> total_sent{0};
        std::atomic<size_t> total_received{0};
        std::atomic<size_t> failed{0};

        // 创建客户端连接
        for (size_t i = 0; i < config.num_connections; ++i) {
            threads.emplace_back([&, i]() {
                try {
                    TcpSocket socket(io_context);
                    socket.connect("127.0.0.1", 8080);

                    for (size_t j = 0; j < config.messages_per_connection; ++j) {
                        TestMessage msg;
                        msg.set_data("test");

                        socket.send(serialize(msg));
                        auto response = socket.receive();

                        if (response) {
                            total_received++;
                        } else {
                            failed++;
                        }
                        total_sent++;
                    }

                } catch (const std::exception& e) {
                    LOG_ERROR("Connection {} failed: {}", i, e.what());
                    failed++;
                }
            });
        }

        // 等待所有线程完成
        for (auto& thread : threads) {
            thread.join();
        }

        // 输出结果
        std::cout << "Total sent: " << total_sent << std::endl;
        std::cout << "Total received: " << total_received << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success rate: "
                  << (100.0 * total_received / total_sent) << "%" << std::endl;
    }
};

int main() {
    StressTest::Config config;
    config.num_connections = 10000;
    config.messages_per_connection = 1000;

    StressTest test;
    test.run(config);

    return 0;
}
```

**压力测试脚本**:

**创建文件**: `scripts/run_stress_test.sh`

```bash
#!/bin/bash

set -e

echo "=== RenduCore Stress Test ==="

# 编译压力测试
cmake --build cmake-build-debug --target stress_test

# 启动服务器
./cmake-build-debug/src/apps/server/server &
SERVER_PID=$!

# 等待服务器启动
sleep 5

# 运行压力测试
echo "Running stress test..."
./cmake-build-debug/src/tests/stress/stress_test

# 停止服务器
kill $SERVER_PID

echo "Stress test completed!"
```

**验收标准**:
- [ ] 支持 10000+ 并发连接
- [ ] 吞吐量 ≥ 100000 消息/秒
- [ ] 72小时稳定运行无崩溃
- [ ] 内存泄漏 < 1MB/小时

---

#### 2.1.2 长时间运行测试

**测试方案**:

```cpp
// tests/longevity/longevity_test.cpp
class LongevityTest {
public:
    void run(std::chrono::hours duration) {
        auto start = std::chrono::system_clock::now();
        auto end = start + duration;

        size_t iteration = 0;
        while (std::chrono::system_clock::now() < end) {
            // 周期性任务
            iteration++;

            // 创建和销毁 Actor
            for (int i = 0; i < 100; ++i) {
                auto actor = system.create<TestActor>(fmt::format("actor_{}", iteration * 100 + i));
                system.destroy(actor.path());
            }

            // 消息传递
            for (int i = 0; i < 1000; ++i) {
                main_actor.tell(TestMessage{});
            }

            // 内存快照
            if (iteration % 100 == 0) {
                auto mem_usage = get_memory_usage();
                LOG_INFO("Iteration: {}, Memory: {} MB", iteration, mem_usage / 1024 / 1024);
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        LOG_INFO("Longevity test completed after {} iterations", iteration);
    }
};
```

**验收标准**:
- [ ] 72小时运行无崩溃
- [ ] 内存增长 < 10MB/小时
- [ ] CPU 使用率稳定
- [ ] 无资源泄漏

---

#### 2.1.3 故障注入测试

**测试方案**:

```cpp
// tests/fault_injection/fault_injection_test.cpp
class FaultInjectionTest {
public:
    void test_network_failure() {
        // 模拟网络故障
        socket.close();
        system.shutdown();

        // 验证恢复能力
        system.restart();
        socket.reconnect();

        // 验证数据一致性
        REQUIRE(system.consistent());
    }

    void test_actor_crash() {
        // 创建会崩溃的 Actor
        auto bad_actor = system.create<CrashingActor>("bad_actor");

        // 触发崩溃
        bad_actor.tell(CrashMessage{});

        // 验证系统恢复
        REQUIRE(system.healthy());
    }

    void test_memory_pressure() {
        // 模拟内存压力
        std::vector<std::vector<char>> buffers;
        for (int i = 0; i < 1000; ++i) {
            buffers.emplace_back(1024 * 1024); // 1MB
        }

        // 验证系统仍能运行
        auto actor = system.create<TestActor>("test_actor");
        actor.tell(TestMessage{});

        // 清理
        buffers.clear();
    }
};
```

---

### 2.2 安全性

#### 2.2.1 输入验证

**实现方案**:

```cpp
// security/input_validator.h
class InputValidator {
public:
    /**
     * @brief 验证消息大小
     */
    static Result<void> validate_message_size(size_t size) {
        static constexpr size_t MAX_MESSAGE_SIZE = 10 * 1024 * 1024; // 10MB
        if (size > MAX_MESSAGE_SIZE) {
            return Error(fmt::format("Message size {} exceeds limit {}", size, MAX_MESSAGE_SIZE));
        }
        return Success();
    }

    /**
     * @brief 验证字符串长度
     */
    static Result<void> validate_string_length(std::string_view str,
                                             size_t max_length = 1024) {
        if (str.length() > max_length) {
            return Error("String length exceeds limit");
        }
        return Success();
    }

    /**
     * @brief 验证路径安全
     */
    static Result<void> validate_path(std::string_view path) {
        // 防止路径遍历攻击
        if (path.find("..") != std::string_view::npos) {
            return Error("Path contains '..'");
        }
        return Success();
    }

    /**
     * @brief 验证 JSON 格式
     */
    static Result<void> validate_json(std::string_view json) {
        if (!JsonSerializer::is_valid(json)) {
            return Error("Invalid JSON format");
        }
        return Success();
    }
};
```

**使用示例**:

```cpp
// 在接收消息时验证
void handle_message(const std::string& data) {
    // 验证消息大小
    auto size_result = InputValidator::validate_message_size(data.size());
    if (!size_result) {
        LOG_ERROR("Invalid message size: {}", size_result.error());
        return;
    }

    // 验证 JSON 格式
    auto json_result = InputValidator::validate_json(data);
    if (!json_result) {
        LOG_ERROR("Invalid JSON: {}", json_result.error());
        return;
    }

    // 处理消息
    process_message(data);
}
```

---

#### 2.2.2 权限控制

**实现方案**:

```cpp
// security/permission_manager.h
class PermissionManager {
public:
    enum class Permission {
        CreateActor,
        DestroyActor,
        SendMessage,
        ReadConfig,
        WriteConfig,
        Shutdown
    };

    /**
     * @brief 检查权限
     */
    bool check_permission(const std::string& user, Permission perm);

    /**
     * @brief 添加用户
     */
    void add_user(const std::string& user,
                  const std::set<Permission>& permissions);

    /**
     * @brief 移除用户
     */
    void remove_user(const std::string& user);

private:
    std::unordered_map<std::string, std::set<Permission>> user_permissions_;
    std::mutex mutex_;
};
```

---

#### 2.2.3 安全编码规范检查

**使用 Clang-Tidy**:

```cmake
# CMakeLists.txt
add_custom_target(safety-check
    COMMAND clang-tidy
        -checks='security-*'
        src/common/**/*.cpp
        src/core/**/*.cpp
        src/apps/**/*.cpp
        --
        -I${CMAKE_SOURCE_DIR}/src/common/include
        -I${CMAKE_SOURCE_DIR}/src/core/include
    COMMENT "Running security checks"
)
```

**常见安全问题检查**:
- [ ] 缓冲区溢出
- [ ] 整数溢出
- [ ] 格式化字符串漏洞
- [ ] SQL 注入 (如使用数据库)
- [ ] 路径遍历
- [ ] 竞态条件

---

### 2.3 工具支持

#### 2.3.1 性能分析工具

**创建工具**: `tools/performance_profiler.sh`

```bash
#!/bin/bash

# 使用 perf 分析 CPU 性能
perf record -g ./cmake-build-debug/src/apps/server/server &
PERF_PID=$!

# 运行负载测试
./scripts/run_stress_test.sh

# 停止 perf
kill -SIGINT $PERF_PID

# 生成报告
perf report -i perf.data

# 生成火焰图
perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > flamegraph.svg
```

**使用 Valgrind 分析内存**:

```bash
#!/bin/bash

# 内存泄漏检测
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --log-file=valgrind.log \
         ./cmake-build-debug/src/apps/server/server

# 检查报告
grep "definitely lost" valgrind.log
grep "indirectly lost" valgrind.log
```

---

#### 2.3.2 调试工具

**创建工具**: `tools/debug_helper.cpp`

```cpp
// tools/debug_helper.h
class DebugHelper {
public:
    /**
     * @brief 打印 Actor 系统状态
     */
    static void print_actor_system_status(ActorSystem& system) {
        auto actors = system.list_actors();
        std::cout << "=== Actor System Status ===" << std::endl;
        std::cout << "Total actors: " << actors.size() << std::endl;

        for (const auto& actor : actors) {
            std::cout << "  " << actor.path << std::endl;
            std::cout << "    Queue size: " << actor.queue_size << std::endl;
            std::cout << "    Processing time: " << actor.avg_time << " ms" << std::endl;
        }
    }

    /**
     * @brief 打印内存使用统计
     */
    static void print_memory_usage() {
        std::ifstream statm("/proc/self/statm");
        size_t size, resident, share, text, lib, data, dt;
        statm >> size >> resident >> share >> text >> lib >> data >> dt;

        std::cout << "=== Memory Usage ===" << std::endl;
        std::cout << "Total: " << (size * 4) / 1024 << " KB" << std::endl;
        std::cout << "Resident: " << (resident * 4) / 1024 << " KB" << std::endl;
        std::cout << "Data: " << (data * 4) / 1024 << " KB" << std::endl;
    }

    /**
     * @brief 打印线程状态
     */
    static void print_thread_status() {
        std::cout << "=== Thread Status ===" << std::endl;
        std::cout << "Active threads: " << std::thread::hardware_concurrency() << std::endl;
        // 打印每个线程的状态
    }
};
```

---

#### 2.3.3 部署脚本

**创建脚本**: `scripts/deploy.sh`

```bash
#!/bin/bash

set -e

# 配置
SERVER_HOST="${1:-user@server}"
REMOTE_DIR="/opt/rendu-core"

echo "=== Deploying to ${SERVER_HOST} ==="

# 1. 编译
echo "Building..."
cmake --build cmake-build-release --target server client

# 2. 打包
echo "Packaging..."
mkdir -p deploy_package
cp cmake-build-release/src/apps/server/server deploy_package/
cp cmake-build-release/src/apps/client/client deploy_package/
cp -r config deploy_package/

# 3. 上传
echo "Uploading..."
scp -r deploy_package/* ${SERVER_HOST}:${REMOTE_DIR}/

# 4. 重启服务
echo "Restarting service..."
ssh ${SERVER_HOST} "sudo systemctl restart rendu-server"

# 5. 验证
echo "Verifying..."
ssh ${SERVER_HOST} "systemctl status rendu-server"

echo "Deployment completed!"
```

---

### 2.4 CI/CD

#### 2.4.1 GitHub Actions 配置

**创建文件**: `.github/workflows/ci.yml`

```yaml
name: CI/CD

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main, develop ]

jobs:
  build:
    runs-on: ubuntu-latest

    strategy:
      matrix:
        compiler: [gcc-9, gcc-11, clang-12]
        build_type: [Debug, Release]

    steps:
    - uses: actions/checkout@v3

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential libboost-all-dev libprotobuf-dev

    - name: Configure CMake
      run: |
        cmake -B build \
              -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
              -DCMAKE_CXX_COMPILER=${{ matrix.compiler }}

    - name: Build
      run: cmake --build build -- -j$(nproc)

    - name: Run tests
      run: ctest --test-dir build --output-on-failure

    - name: Security check
      run: |
        # 运行安全检查工具
        clang-tidy --checks='security-*' src/**/*.cpp

    - name: Upload coverage
      if: matrix.build_type == 'Debug' && matrix.compiler == 'gcc-11'
      run: |
        # 生成覆盖率报告
        cmake --build build --target coverage
        # 上传到 Codecov
        bash <(curl -s https://codecov.io/bash)

  stress-test:
    runs-on: ubuntu-latest
    needs: build

    steps:
    - uses: actions/checkout@v3

    - name: Build stress test
      run: |
        cmake -B build
        cmake --build build --target stress_test

    - name: Run stress test
      run: |
        ./build/src/tests/stress/stress_test

  deploy:
    runs-on: ubuntu-latest
    needs: [build, stress-test]
    if: github.ref == 'refs/heads/main'

    steps:
    - uses: actions/checkout@v3

    - name: Build release
      run: |
        cmake -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build -- -j$(nproc)

    - name: Create package
      run: |
        cpack -G DEB

    - name: Upload artifact
      uses: actions/upload-artifact@v3
      with:
        name: rendu-core-package
        path: rendu-core_*.deb

    - name: Create Release
      if: startsWith(github.ref, 'refs/tags/')
      uses: softprops/action-gh-release@v1
      with:
        files: rendu-core_*.deb
```

---

#### 2.4.2 自动化测试流程

**创建文件**: `.github/workflows/test.yml`

```yaml
name: Tests

on: [push, pull_request]

jobs:
  unit-tests:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v3

    - name: Build
      run: |
        cmake -B build
        cmake --build build

    - name: Run unit tests
      run: ctest --test-dir build -V

  integration-tests:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v3

    - name: Build
      run: |
        cmake -B build
        cmake --build build --target server client

    - name: Start server
      run: |
        ./build/src/apps/server/server &
        sleep 5

    - name: Run integration tests
      run: |
        ./build/src/apps/client/client test

  performance-tests:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v3

    - name: Build
      run: |
        cmake -B build
        cmake --build build --target benchmarks

    - name: Run benchmarks
      run: |
        ./build/src/tests/benchmarks/benchmark_suite

    - name: Check performance regression
      run: |
        # 对比基准性能
        python3 scripts/check_performance.py --baseline baseline.json --current results.json
```

---

## 三、验收标准

- [ ] 压力测试通过 (10000+ 连接)
- [ ] 长时间运行测试通过 (72h+)
- [ ] 故障注入测试通过
- [ ] 安全扫描无高危漏洞
- [ ] 权限控制正常工作
- [ ] 性能分析工具可用
- [ ] CI/CD 流程正常运行
- [ ] 自动化测试覆盖率 ≥ 80%

---

## 四、依赖关系

- 阶段 18 (性能优化)
- 阶段 19 (高级特性)

---

## 五、风险评估

| 风险 | 影响 | 概率 | 应对措施 |
|------|------|------|---------|
| 压力测试资源不足 | 中 | 中 | 使用云服务器测试 |
| 安全漏洞难以发现 | 高 | 低 | 使用专业安全扫描工具 |
| CI/CD 配置复杂 | 低 | 低 | 参考成熟项目配置 |

---

## 六、进度跟踪

| 任务 | 负责人 | 状态 | 预计完成时间 |
|------|--------|------|-------------|
| 压力测试 | boil | ⏳ | 2026-02-25 |
| 长时间运行测试 | boil | ⏳ | 2026-02-26 |
| 故障注入测试 | boil | ⏳ | 2026-02-26 |
| 输入验证 | boil | ⏳ | 2026-02-27 |
| 权限控制 | boil | ⏳ | 2026-02-27 |
| 安全编码检查 | boil | ⏳ | 2026-02-28 |
| 性能分析工具 | boil | ⏳ | 2026-03-01 |
| CI/CD 配置 | boil | ⏳ | 2026-03-02 |

---

## 七、生产检查清单

### 部署前检查

- [ ] 所有单元测试通过
- [ ] 集成测试通过
- [ ] 性能测试达标
- [ ] 安全扫描通过
- [ ] 内存泄漏检测通过
- [ ] 文档完整
- [ ] 配置文件正确
- [ ] 日志级别正确

### 部署后验证

- [ ] 服务正常启动
- [ ] 监控指标正常
- [ ] 健康检查通过
- [ ] 日志正常输出
- [ ] 性能符合预期
- [ ] 无错误告警

---

## 八、备注

- 生产环境需要充分的测试验证
- 安全性是重中之重
- 监控和告警必不可少
- 准备回滚方案

---

**文档版本**: v1.0
**最后更新**: 2026-01-31
