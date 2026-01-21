# 阶段 1: Common 层 - 基础工具 (util)

## 目标
- 实现通用工具函数和类型定义
- 为其他模块提供基础支撑
- 建立代码规范和命名约定

---

## 文件结构

```
src/common/
├── include/common/
│   ├── banner.h
│   ├── define.h
│   ├── git_revision.h
│   ├── util/
│   │   ├── string.h
│   │   ├── time.h
│   │   ├── error.h
│   │   └── container.h
│   └── common.h
└── src/util/
    ├── string.cpp
    ├── time.cpp
    ├── error.cpp
    └── container.cpp
```

---

## 任务清单

### 1. 公共定义 (define.h)
- [ ] 版本宏定义
- [ ] 平台检测宏
- [ ] 编译器检测宏
- [ ] 类型别名（using）
- [ ] 常量定义
- [ ] 调试/断言宏

### 2. 启动横幅 (banner.h)
- [ ] ASCII 艺术横幅
- [ ] 版本信息显示
- [ ] Git 版本号（由 CMake 生成）

### 3. 字符串工具 (string.h/cpp)
- [ ] `trim` - 去除首尾空白
- [ ] `split` - 字符串分割
- [ ] `join` - 字符串连接
- [ ] `to_lower` / `to_upper` - 大小写转换
- [ ] `starts_with` / `ends_with` - 前缀/后缀检查
- [ ] `contains` - 子串检查
- [ ] `replace` - 子串替换

### 4. 时间工具 (time.h/cpp)
- [ ] `now_ms` - 获取毫秒时间戳
- [ ] `now_us` - 获取微秒时间戳
- [ ] `format_time` - 时间格式化
- [ ] `parse_time` - 时间解析
- [ ] `sleep_ms` - 毫秒级休眠
- [ ] `Timer` 类 - 高精度计时器

### 5. 错误处理 (error.h/cpp)
- [ ] `ErrorCode` 枚举 - 错误码定义
- [ ] `Error` 类 - 错误信息封装
- [ ] `Result<T>` - 返回值类型（类似 Rust Result）
- [ ] `RENDU_THROW` - 异常抛出宏
- [ ] `RENDU_TRY` - 错误处理宏

### 6. 容器工具 (container.h/cpp)
- [ ] `contains` - 容器元素检查
- [ ] `erase_if` - 条件删除
- [ ] `find_or` - 查找或返回默认值
- [ ] `map_keys` / `map_values` - 获取键/值集合

---

## 头文件设计

### define.h
```cpp
#pragma once

#include <cstdint>
#include <cstddef>

// 版本
#define RENDU_VERSION_MAJOR 1
#define RENDU_VERSION_MINOR 0
#define RENDU_VERSION_PATCH 0

// 平台
#if defined(_WIN32) || defined(_WIN64)
  #define RENDU_PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define RENDU_PLATFORM_APPLE
#elif defined(__linux__)
  #define RENDU_PLATFORM_LINUX
#endif

// 编译器
#if defined(_MSC_VER)
  #define RENDU_COMPILER_MSVC
#elif defined(__clang__)
  #define RENDU_COMPILER_CLANG
#elif defined(__GNUC__)
  #define RENDU_COMPILER_GCC
#endif

// 类型别名
namespace rendu {
using int8  = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8  = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using size_type = std::size_t;
using byte = std::byte;
}
```

### string.h
```cpp
#pragma once

#include <string>
#include <vector>
#include <string_view>

namespace rendu::str {

std::string trim(std::string_view s);
std::vector<std::string> split(std::string_view s, char delim);
std::string join(const std::vector<std::string>& parts, std::string_view delim);
std::string to_lower(std::string_view s);
std::string to_upper(std::string_view s);
bool starts_with(std::string_view s, std::string_view prefix);
bool ends_with(std::string_view s, std::string_view suffix);
bool contains(std::string_view s, std::string_view sub);
std::string replace(std::string_view s, std::string_view from, std::string_view to);

} // namespace rendu::str
```

### time.h
```cpp
#pragma once

#include <cstdint>
#include <chrono>
#include <string>

namespace rendu::time {

int64_t now_ms();
int64_t now_us();
std::string format_time(int64_t timestamp_ms, const char* format = "%Y-%m-%d %H:%M:%S");
int64_t parse_time(const std::string& time_str, const char* format = "%Y-%m-%d %H:%M:%S");
void sleep_ms(uint32_t ms);

class Timer {
public:
    Timer();
    void reset();
    int64_t elapsed_ms() const;
    int64_t elapsed_us() const;

private:
    std::chrono::steady_clock::time_point start_;
};

} // namespace rendu::time
```

### error.h
```cpp
#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <variant>

namespace rendu {

enum class ErrorCode {
    Success = 0,
    InvalidArgument,
    NotFound,
    AlreadyExists,
    Timeout,
    IOError,
    NetworkError,
    SerializationError,
    Unknown
};

class Error {
public:
    Error(ErrorCode code, std::string_view message = "");
    ErrorCode code() const;
    const std::string& message() const;
    bool is_success() const;

private:
    ErrorCode code_;
    std::string message_;
};

template<typename T>
using Result = std::variant<T, Error>;

#define RENDU_THROW(code, msg) throw rendu::Error(code, msg)

#define RENDU_TRY(expr) \
    do { \
        auto _rendu_result = (expr); \
        if (std::holds_alternative<rendu::Error>(_rendu_result)) { \
            return std::get<rendu::Error>(_rendu_result); \
        } \
    } while(0)

} // namespace rendu
```

### container.h
```cpp
#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>

namespace rendu::container {

template<typename C, typename T>
bool contains(const C& container, const T& value) {
    return std::find(container.begin(), container.end(), value) != container.end();
}

template<typename C, typename Pred>
void erase_if(C& container, Pred pred) {
    container.erase(
        std::remove_if(container.begin(), container.end(), pred),
        container.end()
    );
}

template<typename M, typename K>
auto find_or(const M& map, const K& key, const typename M::mapped_type& default_value) {
    auto it = map.find(key);
    return (it != map.end()) ? it->second : default_value;
}

template<typename M>
auto map_keys(const M& map) -> std::vector<typename M::key_type> {
    std::vector<typename M::key_type> keys;
    keys.reserve(map.size());
    for (const auto& [k, v] : map) {
        keys.push_back(k);
    }
    return keys;
}

template<typename M>
auto map_values(const M& map) -> std::vector<typename M::mapped_type> {
    std::vector<typename M::mapped_type> values;
    values.reserve(map.size());
    for (const auto& [k, v] : map) {
        values.push_back(v);
    }
    return values;
}

} // namespace rendu::container
```

---

## 单元测试

### 测试文件
```
src/tests/common/util/
├── CMakeLists.txt
├── string_test.cpp
├── time_test.cpp
├── error_test.cpp
└── container_test.cpp
```

### CMakeLists.txt
```cmake
rendu_add_test(
    NAME string_test
    SOURCES string_test.cpp
    LINK RenduCore::common
)

rendu_add_test(
    NAME time_test
    SOURCES time_test.cpp
    LINK RenduCore::common
)

rendu_add_test(
    NAME error_test
    SOURCES error_test.cpp
    LINK RenduCore::common
)

rendu_add_test(
    NAME container_test
    SOURCES container_test.cpp
    LINK RenduCore::common
)
```

### string_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/util/string.h>

using namespace rendu::str;

TEST_CASE("string trim", "[util][string]") {
    REQUIRE(trim("  hello  ") == "hello");
    REQUIRE(trim("\thello\t") == "hello");
    REQUIRE(trim("\nhello\n") == "hello");
}

TEST_CASE("string split", "[util][string]") {
    auto parts = split("a,b,c", ',');
    REQUIRE(parts.size() == 3);
    REQUIRE(parts[0] == "a");
    REQUIRE(parts[1] == "b");
    REQUIRE(parts[2] == "c");
}

TEST_CASE("string join", "[util][string]") {
    std::vector<std::string> parts = {"a", "b", "c"};
    REQUIRE(join(parts, ",") == "a,b,c");
}
```

### time_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/util/time.h>
#include <thread>

using namespace rendu::time;

TEST_CASE("time now_ms", "[util][time]") {
    auto t1 = now_ms();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t2 = now_ms();
    REQUIRE(t2 - t1 >= 10);
}

TEST_CASE("Timer elapsed", "[util][time]") {
    Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(timer.elapsed_ms() >= 50);
}
```

---

## 验收标准

### 代码质量
- [ ] 无编译警告
- [ ] 单元测试覆盖率 ≥ 80%
- [ ] 通过 clang-format 检查

### 功能完整性
- [ ] 所有工具函数正确实现
- [ ] 边界情况处理（空字符串、负数等）
- [ ] 线程安全（如需要）

### 文档
- [ ] 所有公开 API 有注释
- [ ] Doxygen 可生成文档

---

## 下一步
完成本阶段后，进入 **阶段 2: Common 层 - I/O 抽象 (io)**
