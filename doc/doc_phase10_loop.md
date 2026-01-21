# 阶段 10: Core 层 - 主循环 (loop)

## 目标
实现主循环抽象，支持可插拔的循环策略。

---

## 任务清单

### 1. loop.h
**文件路径**: `src/core/include/core/loop/loop.h`

**职责**:
- 循环抽象基类
- 定义循环接口

**核心接口**:
```cpp
class ILoop {
public:
    virtual ~ILoop() = default;

    // 运行循环
    virtual void Run() = 0;

    // 停止循环
    virtual void Stop() = 0;

    // 获取帧率
    virtual float GetFPS() const = 0;

    // 获取帧时间
    virtual float GetDeltaTime() const = 0;
};
```

---

### 2. fixed_loop.h
**文件路径**: `src/core/include/core/loop/fixed_loop.h`

**职责**:
- 固定步长循环
- 适合物理模拟

**核心接口**:
```cpp
class FixedLoop : public ILoop {
public:
    FixedLoop(float target_fps = 60.0f);

    void Run() override;
    void Stop() override;

    float GetFPS() const override;
    float GetDeltaTime() const override;

    // 设置更新回调
    void SetUpdateCallback(std::function<void(float)> cb);

    // 设置渲染回调
    void SetRenderCallback(std::function<void()> cb);

private:
    std::chrono::steady_clock::time_point last_time_;
    float target_fps_;
    float fixed_delta_time_;
    std::function<void(float)> update_callback_;
    std::function<void()> render_callback_;
};
```

---

### 3. variable_loop.h
**文件路径**: `src/core/include/core/loop/variable_loop.h`

**职责**:
- 可变步长循环
- 适合普通应用

**核心接口**:
```cpp
class VariableLoop : public ILoop {
public:
    VariableLoop(float max_fps = 60.0f);

    void Run() override;
    void Stop() override;

    float GetFPS() const override;
    float GetDeltaTime() const override;

    // 设置更新回调
    void SetUpdateCallback(std::function<void(float)> cb);

    // 设置最大帧率
    void SetMaxFPS(float fps);

private:
    std::chrono::steady_clock::time_point last_time_;
    float max_fps_;
    std::function<void(float)> update_callback_;
};
```

---

## 设计要点

### 1. 固定步长循环
- 固定时间步长更新
- 渲染帧率可变
- 适合物理模拟

### 2. 可变步长循环
- 实时计算 delta time
- 简单易用
- 适合普通应用

### 3. 帧率控制
- 目标帧率设置
- 最大帧率限制
- 帧率平滑

### 4. 性能优化
- 高精度计时
- 避免忙等待
- 自适应休眠

---

## 验收标准

- [ ] 固定步长循环稳定
- [ ] 可变步长循环正确
- [ ] 帧率符合预期
- [ ] 循环可正常停止
- [ ] 性能满足预期
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 9 (engine) - 引擎核心
- 阶段 2 (io) - io_context
- RenduCore::common - 公共层
