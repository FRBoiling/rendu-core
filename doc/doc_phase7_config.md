# 阶段 7: Common 层 - 配置管理 (config)

## 目标
提供配置加载能力，支持配置热更新。

---

## 任务清单

### 1. config.h
**文件路径**: `src/common/include/common/config/config.h`

**职责**:
- 配置数据结构定义
- 类型安全的配置访问

**核心接口**:
```cpp
class Config {
public:
    // 获取字符串
    std::string GetString(const std::string& key,
                         const std::string& default_value = "");

    // 获取整数
    int GetInt(const std::string& key, int default_value = 0);

    // 获取浮点数
    double GetDouble(const std::string& key, double default_value = 0.0);

    // 获取布尔值
    bool GetBool(const std::string& key, bool default_value = false);

    // 设置值
    void Set(const std::string& key, const std::string& value);
    void Set(const std::string& key, int value);
    void Set(const std::string& key, double value);
    void Set(const std::string& key, bool value);

    // 保存配置
    bool Save(const std::string& filename);
};
```

---

### 2. loader.h
**文件路径**: `src/common/include/common/config/loader.h`

**职责**:
- 配置加载器
- 支持多种格式（JSON/YAML/INI）

**核心接口**:
```cpp
class ConfigLoader {
public:
    // 加载配置文件
    static std::shared_ptr<Config> Load(const std::string& filename);

    // 设置默认配置
    static void SetDefaults(std::shared_ptr<Config> config);

    // 验证配置
    static bool Validate(const Config& config);
};
```

**支持的格式**:
- JSON
- YAML（可选）
- INI（可选）

---

### 3. watcher.h
**文件路径**: `src/common/include/common/config/watcher.h`

**职责**:
- 配置文件监控
- 热更新通知

**核心接口**:
```cpp
class ConfigWatcher {
public:
    using ChangeCallback = std::function<void(const Config&)>;

    // 启动监控
    void Start(const std::string& filename, ChangeCallback callback);

    // 停止监控
    void Stop();

    // 设置检查间隔
    void SetCheckInterval(std::chrono::milliseconds interval);
};
```

---

## 设计要点

### 1. 配置格式
- 优先使用 JSON（易读、易解析）
- 支持 YAML（更复杂配置）
- 支持 INI（简单配置）

### 2. 热更新
- 文件变化检测
- 自动重新加载
- 通知订阅者

### 3. 类型安全
- 编译期类型检查
- 运行时类型转换
- 默认值处理

### 4. 配置验证
- 必填项检查
- 范围检查
- 格式检查

---

## 验收标准

- [ ] JSON 配置加载正确
- [ ] 类型转换正常
- [ ] 热更新及时生效
- [ ] 配置验证正确
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 5 (ser) - 序列化
- 阶段 6 (event) - 配置变更事件
- 阶段 2 (io) - 文件监控
