# 阶段 11: Core 层 - 状态管理 (state)

## 目标
实现状态机，管理运行时状态。

---

## 任务清单

### 1. state_machine.h
**文件路径**: `src/core/include/core/state/state_machine.h`

**职责**:
- 状态机
- 管理状态转换

**核心接口**:
```cpp
class StateMachine {
public:
    // 切换状态
    void ChangeState(std::shared_ptr<IState> state);

    // 推入状态（栈式）
    void PushState(std::shared_ptr<IState> state);

    // 弹出状态
    void PopState();

    // 更新当前状态
    void Update(float delta_time);

    // 获取当前状态
    std::shared_ptr<IState> GetCurrentState();

    // 获取所有状态
    const std::vector<std::shared_ptr<IState>>& GetStates() const;
};
```

---

### 2. state.h
**文件路径**: `src/core/include/core/state/state.h`

**职责**:
- 状态基类
- 定义状态生命周期

**核心接口**:
```cpp
class IState {
public:
    virtual ~IState() = default;

    // 状态名称
    virtual std::string GetName() const = 0;

    // 进入状态
    virtual void OnEnter();

    // 离开状态
    virtual void OnExit();

    // 暂停状态
    virtual void OnPause();

    // 恢复状态
    virtual void OnResume();

    // 更新
    virtual void Update(float delta_time) = 0;
};

// 状态基类
class StateBase : public IState {
public:
    StateBase(const std::string& name);

    std::string GetName() const override;

protected:
    std::string name_;
};
```

---

## 设计要点

### 1. 状态转换
- 支持单状态切换
- 支持状态栈（菜单/游戏/暂停等）
- 自动处理 OnEnter/OnExit

### 2. 生命周期
- OnEnter: 进入状态时调用
- OnExit: 离开状态时调用
- OnPause: 被暂停时调用
- OnResume: 从暂停恢复时调用

### 3. 状态共享数据
- 通过 Context 共享数据
- 避免全局变量

### 4. 状态持久化
- 支持状态序列化
- 支持状态恢复

---

## 验收标准

- [ ] 状态转换正确
- [ ] 状态栈工作正常
- [ ] OnEnter/OnExit 调用正确
- [ ] OnPause/OnResume 调用正确
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 6 (event) - 事件通知
- RenduCore::common - 公共层
