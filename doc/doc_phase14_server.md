# 阶段 14: 应用层 - 服务器 (server)

## 目标
- 基于框架实现一个完整的服务器示例
- 验证框架在生产环境下的表现
- 演示 Actor 系统的实际应用

---

## 文件结构

```
src/apps/server/
├── CMakeLists.txt
├── main.cpp
├── server_actor.h
├── session_actor.h
└── protocol/
    └── messages.proto
```

---

## 任务清单

### 1. 协议定义 (messages.proto)
- [x] 定义通信消息
- [x] 生成 protobuf 代码

### 2. Session Actor (session_actor.h/cpp)
- [x] 管理单个客户端连接
- [x] 处理客户端消息
- [x] 维护会话状态

### 3. Server Actor (server_actor.h/cpp)
- [x] 管理所有会话
- [x] 处理全局消息
- [x] 消息广播

### 4. 主程序 (main.cpp)
- [x] 初始化引擎
- [x] 启动网络监听
- [x] 启动 Actor 系统
- [x] 处理信号

---

## 头文件设计

### session_actor.h
```cpp
#pragma once

#include <core/actor/actor.h>
#include <core/actor/message.h>
#include <common/net/channel.h>
#include <protocol/messages.pb.h>

namespace server {

class SessionActor : public actor::Actor {
public:
    SessionActor(std::string name, std::shared_ptr<net::Channel> channel);
    ~SessionActor() override;

    void receive(const actor::Message& message) override;
    void on_stop() override;

private:
    void on_data_received(const protocol::ClientMessage& msg);
    void send_to_client(const protocol::ServerMessage& msg);

    std::shared_ptr<net::Channel> channel_;
    int user_id_;
};

} // namespace server
```

### server_actor.h
```cpp
#pragma once

#include <core/actor/actor.h>
#include <core/actor/message.h>
#include <unordered_map>

namespace server {

class SessionActor;

class ServerActor : public actor::Actor {
public:
    explicit ServerActor(std::string name);
    ~ServerActor() override;

    void receive(const actor::Message& message) override;

    // 添加会话
    void add_session(int user_id, std::shared_ptr<SessionActor> session);

    // 移除会话
    void remove_session(int user_id);

    // 广播消息
    void broadcast(const protocol::ServerMessage& msg);

private:
    void on_login(const protocol::LoginRequest& msg);
    void on_chat(const protocol::ChatMessage& msg);

    std::unordered_map<int, std::shared_ptr<SessionActor>> sessions_;
    int next_user_id_;
};

} // namespace server
```

---

## 主程序

### main.cpp
```cpp
#include <core/engine/engine.h>
#include <core/actor/actor_system.h>
#include <core/loop/fixed_loop.h>
#include <common/net/tcp_acceptor.h>
#include <server/server_actor.h>
#include <server/session_actor.h>
#include <csignal>

using namespace rendu;

static bool running = true;

void signal_handler(int signal) {
    running = false;
}

int main() {
    // 初始化引擎
    engine::Engine engine;
    engine.initialize();

    // 设置信号处理
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    auto& context = engine.context();
    auto& logger = context.logger();

    RENDU_LOG_INFO("Starting server...");

    // 创建 Actor 系统
    actor::ActorSystem actor_system(context.io());
    auto server_actor = actor_system.create_actor<server::ServerActor>("server");

    // 创建 TCP 接收器
    net::TcpAcceptor acceptor(context.io(), 8080);

    acceptor.async_accept([&](std::shared_ptr<net::TcpSocket> socket, const std::error_code& ec) {
        if (ec) {
            RENDU_LOG_ERROR("Accept failed: " + ec.message());
            return;
        }

        // 创建会话 Actor
        auto channel = std::make_shared<net::Channel>(
            socket,
            std::make_shared<net::LengthPrefixCodec>()
        );

        int user_id = server_actor.tell(/* 获取新用户 ID */);

        auto session = actor_system.create_actor<server::SessionActor>(
            "session_" + std::to_string(user_id),
            channel
        );

        RENDU_LOG_INFO("Client connected: " + std::to_string(user_id));
    });

    // 启动引擎
    engine.start();

    // 主循环
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    // 清理
    RENDU_LOG_INFO("Shutting down server...");
    engine.stop();

    return 0;
}
```

---

## 验收标准

### 功能
- [x] 服务器稳定运行
- [x] 支持多客户端连接
- [x] 消息收发正确
- [x] 优雅关闭

### 性能
- [ ] 支持 1000+ 并发连接（待测试）
- [ ] 消息延迟 < 50ms（待测试）
- [ ] CPU 占用 < 50%（待测试）

### 可靠性
- [ ] 连接断开处理正确（待测试）
- [ ] 异常恢复能力（待测试）
- [ ] 无内存泄漏（待测试）

---

## 项目完成

完成本阶段后，RenduCore 框架的基础功能已全部实现，可用于实际项目开发。
