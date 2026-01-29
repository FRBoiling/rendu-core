# RenduCore 服务器示例

这是一个基于 RenduCore 框架实现的完整服务器示例，演示了：

- Actor 模型的实际应用
- 网络通信和会话管理
- Protobuf 消息序列化
- 多客户端连接处理

## 项目结构

```
src/apps/server/
├── CMakeLists.txt           # CMake 配置
├── README.md               # 本文档
├── main.cpp                # 主程序入口
├── server_actor.h/cpp      # 服务器 Actor（管理所有会话）
├── session_actor.h/cpp     # 会话 Actor（管理单个客户端连接）
├── server_messages.h        # Actor 间通信消息定义
└── protocol/
    └── messages.proto      # Protobuf 协议定义
```

## 编译

```bash
cd build
cmake ..
cmake --build . --target server
```

生成的可执行文件位于：`build/bin/server`

## 运行

```bash
./build/bin/server
```

服务器将监听 8080 端口，支持多个客户端同时连接。

## 协议说明

### 登录

客户端发送：
```protobuf
ClientMessage {
  login: LoginRequest {
    username: "player1"
    password: "password"
  }
}
```

服务器响应：
```protobuf
ServerMessage {
  login_response: LoginResponse {
    success: true
    user_id: 1
    message: "Login successful"
  }
}
```

### 聊天

客户端发送：
```protobuf
ClientMessage {
  chat: ChatMessage {
    from_user_id: 1
    from_username: "player1"
    content: "Hello!"
    timestamp: 1640908800
  }
}
```

服务器将广播此消息给所有其他客户端。

## 功能演示

1. **登录认证**：客户端发送用户名和密码进行登录
2. **聊天消息**：登录后的用户可以发送聊天消息，服务器广播给所有其他用户
3. **会话管理**：每个客户端连接对应一个 Session Actor
4. **消息路由**：Server Actor 管理所有会话并协调消息传递

## 测试建议

使用 telnet 或编写简单的客户端程序测试：

```bash
telnet localhost 8080
```

## 性能指标

- 支持多客户端并发连接
- 基于 Actor 模型的消息传递
- 异步 I/O 处理
- Protobuf 高效序列化
