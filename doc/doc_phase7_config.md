# 阶段 7: Common 层 - 配置管理 (config)

**完成日期**: 2026-01-26
**状态**: ✅ 完成（watcher 未实现，非核心功能）

---

## 目标
- 提供配置加载能力
- 支持 JSON 格式配置文件
- 类型安全的配置访问
- 支持嵌套配置结构

---

## 文件结构

```
src/common/
├── include/common/config/
│   ├── config.h       - 配置数据结构和访问接口
│   └── loader.h       - 配置加载器
└── src/config/
    ├── config.cpp
    ├── loader.cpp
    └── watcher.cpp    - 暂未实现
```

---

## 实现状态

### 1. 配置数据结构 (config.h/cpp) ✅
- [x] ConfigValue 类型支持多种数据类型
  - 基本类型: int64_t, double, bool, std::string
  - 数组类型: vector<int64_t>, vector<double>, vector<bool>, vector<string>
  - 对象类型: map<string, int64_t>, map<string, double>, map<string, bool>, map<string, string>
- [x] Config 类提供类型安全的访问接口
- [x] 支持点号分隔的嵌套路径（如 "database.port"）
- [x] 支持默认值获取（get_or_default）
- [x] 支持配置合并

### 2. 配置加载器 (loader.h/cpp) ✅
- [x] JsonLoader 实现
- [x] Loader 抽象基类（便于扩展其他格式）
- [ ] YamlLoader - 未实现（可选）
- [ ] IniLoader - 未实现（可选）

### 3. 配置监控 (watcher.h/cpp) ⚠️
- [ ] 文件变更检测
- [ ] 热更新回调
- [ ] 防抖处理
- **状态**: 暂未实现，属于非核心功能

---

## 核心设计

### ConfigValue 类型

为了简化实现，数组类型和对象类型中的值限制为基本类型：

```cpp
using ConfigValue = std::variant<
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::unordered_map<std::string, int64_t>,
    std::unordered_map<std::string, double>,
    std::unordered_map<std::string, bool>,
    std::unordered_map<std::string, std::string>
>;
```

### Config 类特性

1. **类型安全访问**
   - `template<typename T> Result<T> get(const std::string& key)`
   - 编译时类型检查
   - 返回 Result<T> 包含值或错误

2. **嵌套路径支持**
   - 支持 `database.port` 形式的点号分隔路径
   - 自动解析多层嵌套结构

3. **默认值支持**
   - `template<typename T> T get_or_default(const std::string& key, const T& default_value)`
   - 配置不存在时返回默认值

4. **配置合并**
   - `void merge(const Config& other)`
   - 用于覆盖默认配置

---

## 使用示例

```cpp
#include <common/config/config.h>
#include <common/config/loader.h>

using namespace rendu::common;

// 加载 JSON 配置文件
config::JsonLoader loader;
auto result = loader.load("config.json");

if (!std::holds_alternative<config::Config>(result)) {
    auto error = std::get<util::Error>(result);
    LOG_ERROR("Failed to load config: {}", error.message());
    return;
}

auto config = std::get<config::Config>(result);

// 获取配置值
auto port = config.get<int64_t>("server.port");
if (std::holds_alternative<int64_t>(port)) {
    LOG_INFO("Server port: {}", std::get<int64_t>(port));
}

// 获取带默认值的配置
auto timeout = config.get_or_default<int64_t>("server.timeout", 30);
auto debug = config.get_or_default<bool>("server.debug", false);

// 检查配置是否存在
if (config.has("database.host")) {
    auto host = config.get<std::string>("database.host");
}

// 设置配置值
config.set("server.port", 8080);
config.set("server.debug", true);

// 遍历所有键
auto keys = config.keys();
for (const auto& key : keys) {
    LOG_INFO("Config key: {}", key);
}
```

---

## JSON 配置文件示例

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "debug": true,
    "timeout": 30
  },
  "database": {
    "host": "localhost",
    "port": 5432,
    "name": "mydb",
    "user": "admin"
  },
  "features": ["feature1", "feature2", "feature3"],
  "limits": {
    "max_connections": 1000,
    "max_requests": 10000
  }
}
```

---

## 单元测试

### 测试统计
| 测试文件 | 测试用例 | 断言数 | 状态 |
|---------|----------|---------|------|
| config_test.cpp | 8 | 29 | ✅ 通过 |
| loader_test.cpp | 4 | 16 | ✅ 通过 |
| **总计** | **12** | **45** | **✅ 100%** |

### 测试覆盖
- ✅ 基本类型获取和设置
- ✅ 嵌套路径访问
- ✅ 默认值处理
- ✅ 类型转换和类型检查
- ✅ 配置合并
- ✅ JSON 文件加载
- ✅ 错误处理（文件不存在、格式错误）
- ✅ 数组和对象类型

---

## 限制与改进方向

### 当前限制
1. **嵌套限制**: 数组和对象中的值限制为基本类型，不支持嵌套
2. **热更新缺失**: ConfigWatcher 未实现，不支持配置文件热更新
3. **格式支持**: 仅支持 JSON 格式

### 改进方向
1. **实现 ConfigWatcher**: 基于文件系统监控实现热更新
2. **支持更多格式**: YAML、TOML、INI 等
3. **完整嵌套支持**: 允许数组和对象中包含任意 ConfigValue
4. **配置验证**: 添加配置项验证和默认值模板

---

## 依赖关系
- 阶段 1 (util) - Error 类型
- 阶段 5 (ser) - JSON 解析（可选，可用于高级功能）

---

## 下一阶段
完成本阶段后，进入 **阶段 8: Common 层 - ECS 系统 (ecs)**
