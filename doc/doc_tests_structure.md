# Tests 层结构文档

## 目录结构

```
src/tests/
├── CMakeLists.txt                  # 测试总配置
├── common/                        # Common 层测试
│   ├── util/
│   │   ├── CMakeLists.txt
│   │   ├── string_test.cpp
│   │   ├── time_test.cpp
│   │   ├── error_test.cpp
│   │   └── container_test.cpp
│   ├── io/
│   │   ├── CMakeLists.txt
│   │   ├── io_context_test.cpp
│   │   ├── scheduler_test.cpp
│   │   └── timer_test.cpp
│   ├── log/
│   │   ├── CMakeLists.txt
│   │   ├── logger_test.cpp
│   │   ├── sink_test.cpp
│   │   ├── console_sink_test.cpp
│   │   ├── file_sink_test.cpp
│   │   └── formatter_test.cpp
│   ├── net/
│   │   ├── CMakeLists.txt
│   │   ├── socket_test.cpp
│   │   ├── channel_test.cpp
│   │   └── codec_test.cpp
│   ├── ser/
│   │   ├── CMakeLists.txt
│   │   ├── protobuf_ser_test.cpp
│   │   └── json_ser_test.cpp
│   ├── event/
│   │   ├── CMakeLists.txt
│   │   ├── event_test.cpp
│   │   ├── event_bus_test.cpp
│   │   └── handler_test.cpp
│   ├── config/
│   │   ├── CMakeLists.txt
│   │   ├── config_test.cpp
│   │   ├── loader_test.cpp
│   │   └── watcher_test.cpp
│   └── ecs/
│       ├── CMakeLists.txt
│       ├── registry_test.cpp
│       ├── system_test.cpp
│       └── view_test.cpp
└── core/                          # Core 层测试
    ├── engine/
    │   ├── CMakeLists.txt
    │   ├── engine_test.cpp
    │   └── context_test.cpp
    ├── loop/
    │   ├── CMakeLists.txt
    │   ├── fixed_loop_test.cpp
    │   └── variable_loop_test.cpp
    ├── state/
    │   ├── CMakeLists.txt
    │   ├── state_machine_test.cpp
    │   └── state_test.cpp
    ├── lifecycle/
    │   ├── CMakeLists.txt
    │   ├── lifecycle_test.cpp
    │   └── lifecycle_manager_test.cpp
    └── actor/
        ├── CMakeLists.txt
        ├── actor_test.cpp
        ├── actor_system_test.cpp
        ├── actor_ref_test.cpp
        ├── message_test.cpp
        └── message_queue_test.cpp
```

---

## 测试框架

使用 **Catch2** 作为测试框架。

---

## CMakeLists.txt 模板

### tests/CMakeLists.txt
```cmake
rendu_add_subdirectories(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

### tests/common/util/CMakeLists.txt
```cmake
rendu_add_test(
    NAME string_test
    SOURCES string_test.cpp
    LINK
        RenduCore::common
)

rendu_add_test(
    NAME time_test
    SOURCES time_test.cpp
    LINK
        RenduCore::common
)
```

---

## 测试命名规范

- 文件名: `{module}_test.cpp`
- 测试用例: `TEST_CASE("{feature}", "[{module}]")`
- 测试描述: `SCENARIO("...", "[{module}]")`

---

## 测试覆盖率目标

- **代码覆盖率 ≥ 80%**
- **关键路径覆盖率 = 100%**
- **分支覆盖率 ≥ 70%**

---

## 运行测试

```bash
# 运行所有测试
ctest

# 运行特定测试
./string_test

# 运行特定模块测试
ctest -R util

# 输出详细
ctest -V

# 输出覆盖率
cmake --build . --target coverage
```

---

## 持续集成

每次提交自动运行：
- 单元测试
- 代码覆盖率检查
- 静态分析
- 内存泄漏检测（Valgrind）
