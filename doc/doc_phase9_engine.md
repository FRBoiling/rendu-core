# 阶段 9: Core 层 - 引擎核心 (engine)

## 目标
实现主引擎类，管理引擎生命周期。

---

## 任务清单

### 1. engine.h
**文件路径**: `src/core/include/core/engine/engine.h`

**职责**:
- 引擎主类
- 管理所有子系统
- 控制引擎生命周期

**核心接口**:
```cpp
class Engine {
public:
    Engine();
    ~Engine();

    // 初始化引擎
    bool Initialize();

    // 运行引擎
    void Run();

    // 停止引擎
    void Stop();

    // 获取引擎上下文
    Context& GetContext();

    // 添加系统
    void AddSystem(std::shared_ptr<ISystem> system);

    // 移除系统
    void RemoveSystem(const std::string& name);

    // 获取系统
    template<typename System>
    std::shared_ptr<System> GetSystem();

private:
    std::unique_ptr<Context> context_;
    std::vector<std::shared_ptr<ISystem>> systems_;
    bool running_;
};
```

---

### 2. context.h
**文件路径**: `src/core/include/core/engine/context.h`

**职责**:
- 引擎上下文
- 管理全局状态和资源

**核心接口**:
```cpp
class Context {
public:
    // 获取 io_context
    common::IOContext& GetIOContext();

    // 获取日志系统
    common::Logger& GetLogger();

    // 获取 ECS 注册表
    common::Registry& GetRegistry();

    // 获取事件总线
    common::EventBus& GetEventBus();

    // 获取配置
    common::Config& GetConfig();

private:
    std::unique_ptr<common::IOContext> io_context_;
    std::shared_ptr<common::Registry> registry_;
    common::EventBus& event_bus_;
    std::shared_ptr<common::Config> config_;
};
```

---

## 设计要点

### 1. 生命周期管理
- Initialize: 初始化所有子系统
- Run: 进入主循环
- Stop: 优雅停止，清理资源

### 2. 子系统管理
- 系统按顺序初始化
- 系统按顺序更新
- 系统按优先级执行

### 3. 上下文共享
- 统一的上下文对象
- 所有系统通过上下文访问资源

### 4. 错误处理
- 初始化失败的处理
- 运行时异常的捕获
- 优雅退出

---

## 验收标准

- [ ] 引擎可正常启动
- [ ] 子系统正确初始化
- [ ] 引擎可正常停止
- [ ] 资源正确释放
- [ ] 异常处理正确
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 2 (io) - io_context
- 阶段 3 (log) - 日志系统
- 阶段 6 (event) - 事件总线
- 阶段 8 (ecs) - ECS 系统
- RenduCore::common - 公共层
