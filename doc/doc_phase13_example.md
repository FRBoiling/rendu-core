# 阶段 13: 应用层 - 示例程序 (example)

## 目标
演示框架使用方法，验证各模块集成。

---

## 任务清单

### 1. 日志示例
**文件路径**: `src/apps/example/logger_example.cpp`

**内容**:
- 初始化日志系统
- 添加多个 Sink
- 输出不同级别的日志
- 测试日志格式化
- 测试文件轮转

---

### 2. 网络示例
**文件路径**: `src/apps/example/network_example.cpp`

**内容**:
- 创建 TCP 服务器
- 创建 TCP 客户端
- 实现消息收发
- 实现自定义编解码器
- 测试断线重连

---

### 3. 事件示例
**文件路径**: `src/apps/example/event_example.cpp`

**内容**:
- 定义自定义事件
- 订阅事件
- 发布事件（同步/异步）
- 测试事件分发
- 测试事件取消订阅

---

### 4. ECS 示例
**文件路径**: `src/apps/example/ecs_example.cpp`

**内容**:
- 创建实体和组件
- 实现系统
- 创建视图查询
- 测试系统更新
- 性能测试

---

### 5. 引擎示例
**文件路径**: `src/apps/example/engine_example.cpp`

**内容**:
- 创建引擎
- 初始化引擎
- 运行主循环
- 停止引擎
- 清理资源

---

### 6. Actor 示例
**文件路径**: `src/apps/example/actor_example.cpp`

**内容**:
- 创建 Actor
- 实现 Actor 消息处理
- Actor 间消息传递
- ActorRef 使用
- Actor 池使用

---

## 验收标准

- [ ] 所有示例可编译
- [ ] 所有示例可正常运行
- [ ] 示例代码清晰易懂
- [ ] 示例有详细注释
- [ ] 示例有 README 说明

---

## 依赖
- 阶段 12.5 (actor) - Actor 系统
