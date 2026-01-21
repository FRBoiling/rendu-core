# 阶段 7: Common 层 - 配置管理 (config)

## 目标
- 提供配置加载能力
- 支持多种格式（JSON/YAML/INI）
- 支持配置热更新

---

## 文件结构

```
src/common/
├── include/common/config/
│   ├── config.h
│   ├── loader.h
│   └── watcher.h
└── src/config/
    ├── config.cpp
    ├── loader.cpp
    └── watcher.cpp
```

---

## 任务清单

### 1. 配置数据结构 (config.h/cpp)
- [ ] 统一的配置值类型
- [ ] 支持嵌套结构
- [ ] 类型安全的访问接口

### 2. 配置加载器 (loader.h/cpp)
- [ ] JSON 加载
- [ ] YAML 加载（可选）
- [ ] INI 加载（可选）

### 3. 配置监控 (watcher.h/cpp)
- [ ] 文件变更检测
- [ ] 热更新回调
- [ ] 防抖处理

---

## 头文件设计

### config.h
```cpp
#pragma once

#include <string>
#include <variant>
#include <unordered_map>
#include <vector>
#include <common/util/error.h>

namespace rendu::config {

using ConfigValue = std::variant<
    int,
    double,
    bool,
    std::string,
    std::vector<ConfigValue>,
    std::unordered_map<std::string, ConfigValue>
>;

class Config {
public:
    Config() = default;

    // 获取配置值
    template<typename T>
    Result<T> get(const std::string& key) const {
        auto value = find_value(key);
        if (!value) {
            return Error(ErrorCode::NotFound, "Key not found: " + key);
        }
        return std::get_if<T>(&value);
    }

    // 设置配置值
    void set(const std::string& key, const ConfigValue& value);

    // 检查是否存在
    bool has(const std::string& key) const;

    // 合并配置
    void merge(const Config& other);

private:
    const ConfigValue* find_value(const std::string& key) const;

    std::unordered_map<std::string, ConfigValue> data_;
};

} // namespace rendu::config
```

### loader.h
```cpp
#pragma once

#include <string>
#include <memory>
#include <common/config/config.h>

namespace rendu::config {

class Loader {
public:
    virtual ~Loader() = default;
    virtual Result<Config> load(const std::string& filepath) = 0;
};

class JsonLoader : public Loader {
public:
    Result<Config> load(const std::string& filepath) override;
};

class YamlLoader : public Loader { // 可选
public:
    Result<Config> load(const std::string& filepath) override;
};

class IniLoader : public Loader { // 可选
public:
    Result<Config> load(const std::string& filepath) override;
};

} // namespace rendu::config
```

### watcher.h
```cpp
#pragma once

#include <string>
#include <functional>
#include <memory>
#include <common/io/io_context.h>

namespace rendu::config {

using WatchCallback = std::function<void(const Config&)>;

class ConfigWatcher {
public:
    explicit ConfigWatcher(IoContext& io);
    ~ConfigWatcher();

    // 监控文件
    void watch(const std::string& filepath, WatchCallback callback);

    // 停止监控
    void stop();

private:
    void check_file();

    IoContext& io_;
    std::string filepath_;
    WatchCallback callback_;
    std::string last_hash_;
};

} // namespace rendu::config
```

---

## 单元测试

### 测试文件
```
src/tests/common/config/
├── CMakeLists.txt
├── config_test.cpp
├── loader_test.cpp
└── watcher_test.cpp
```

### loader_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/config/loader.h>

using namespace rendu::config;

TEST_CASE("JsonLoader load", "[config][loader]") {
    JsonLoader loader;
    // 假设有 test.json 文件
    auto result = loader.load("test.json");

    REQUIRE(std::holds_alternative<Config>(result));
    auto config = std::get<Config>(result);

    REQUIRE(config.has("key1"));
}
```

---

## 验收标准

### 功能
- [ ] 配置加载正确
- [ ] 支持多种格式
- [ ] 热更新及时生效
- [ ] 类型转换正确

### 可靠性
- [ ] 文件不存在时返回错误
- [ ] 格式错误时返回错误
- [ ] 防抖处理避免频繁更新

---

## 下一步
完成本阶段后，进入 **阶段 8: Common 层 - ECS 系统 (ecs)**
