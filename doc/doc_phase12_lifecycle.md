# 阶段 12: Core 层 - 生命周期 (lifecycle)

## 目标
定义组件生命周期，实现统一的初始化/销毁流程。

---

## 任务清单

### 1. lifecycle.h
**文件路径**: `src/core/include/core/lifecycle/lifecycle.h`

**职责**:
- 生命周期接口定义
- 统一的初始化/销毁流程

**核心接口**:
```cpp
class ILifecycle {
public:
    virtual ~ILifecycle() = default;

    // 初始化
    virtual bool Initialize() = 0;

    // 启动
    virtual void Startup() {}

    // 更新
    virtual void Update(float delta_time) {}

    // 停止
    virtual void Shutdown() {}

    // 清理
    virtual void Cleanup() = 0;

    // 是否已初始化
    bool IsInitialized() const;

    // 是否正在运行
    bool IsRunning() const;

protected:
    bool initialized_ = false;
    bool running_ = false;
};

// 生命周期管理器
class LifecycleManager {
public:
    // 注册生命周期组件
    void Register(std::shared_ptr<ILifecycle> component);

    // 注销生命周期组件
    void Unregister(const std::string& name);

    // 初始化所有组件
    bool InitializeAll();

    // 启动所有组件
    void StartupAll();

    // 更新所有组件
    void UpdateAll(float delta_time);

    // 停止所有组件
    void ShutdownAll();

    // 清理所有组件
    void CleanupAll();

private:
    std::vector<std::shared_ptr<ILifecycle>> components_;
};
```

---

## 设计要点

### 1. 生命周期阶段
- Initialize: 初始化，分配资源
- Startup: 启动，开始运行
- Update: 每帧更新
- Shutdown: 停止运行
- Cleanup: 清理，释放资源

### 2. 初始化顺序
- 按注册顺序初始化
- 确保依赖关系正确

### 3. 错误处理
- Initialize 返回 bool
- 初始化失败时的处理
- 部分初始化成功的清理

### 4. 状态跟踪
- 跟踪初始化状态
- 跟踪运行状态

---

## 验收标准

- [ ] 初始化顺序正确
- [ ] 清理顺序正确（反初始化）
- [ ] 状态跟踪正确
- [ ] 错误处理正确
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 9 (engine) - 引擎核心
- RenduCore::common - 公共层
