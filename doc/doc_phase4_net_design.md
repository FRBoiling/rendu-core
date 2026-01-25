# 阶段 4: Common 层 - 网络通信 (net) - 设计文档

## 1. 模块概述

### 1.1 目标
构建高性能、可扩展、易用的异步网络通信模块，为上层应用提供统一的网络编程接口。

### 1.2 核心特性
- **异步 I/O**: 基于 Boost.Asio 的非阻塞 I/O 模型
- **协议支持**: TCP（核心）、UDP（扩展）
- **编解码器**: 可插拔的编解码器设计
- **高性能**: 零拷贝优化、批量发送
- **线程安全**: 支持多线程并发操作
- **错误处理**: 完善的错误处理和恢复机制

---

## 2. 架构设计

### 2.1 分层架构

```
┌─────────────────────────────────────────┐
│         应用层 (Apps/Core)          │
│  使用 Channel 和 ChannelFactory       │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│       Channel 层 (channel.h)        │
│  连接管理、消息编解码、回调         │
└──────────────┬──────────────────────┘
               │
        ┌──────┴──────┐
        │             │
┌───────▼────┐  ┌───▼──────┐
│  Socket层   │  │ Codec层   │
│ (socket.h)  │  │(codec.h) │
└──────┬──────┘  └───────────┘
       │
┌──────▼──────────────────────────┐
│     IoContext 层 (io)          │
│   事件循环、线程池             │
└─────────────────────────────────┘
```

### 2.2 模块职责

#### Socket 层
- 封装原始 Socket 操作
- 处理底层连接、发送、接收
- 提供异步接口

#### Channel 层
- 管理连接生命周期
- 协议编解码
- 提供应用层回调接口
- 缓冲区管理

#### Codec 层
- 抽象编解码接口
- 实现常见编解码协议
- 支持自定义扩展

---

## 3. 详细设计

### 3.1 Socket 层设计

#### 3.1.1 TcpSocket

**职责**: 封装 TCP Socket，提供异步连接、发送、接收功能

**核心功能**:
1. **异步连接**
   - 支持域名解析连接
   - 支持 endpoint 连接
   - 超时控制

2. **异步发送**
   - 支持零拷贝发送
   - 批量发送优化
   - 发送队列管理

3. **异步接收**
   - 固定长度接收
   - 动态长度接收
   - 接收缓冲区复用

4. **连接管理**
   - 连接状态跟踪
   - 优雅关闭
   - 异常检测

**错误处理**:
```cpp
enum class SocketError {
    Success = 0,
    ConnectionRefused,
    ConnectionTimeout,
    ConnectionReset,
    Disconnected,
    SendFailed,
    ReceiveFailed,
    InvalidEndpoint,
    AlreadyConnected,
    NotConnected,
    BufferOverflow,
    Unknown
};
```

**性能优化**:
- 使用缓冲区池减少内存分配
- 发送队列合并小包
- 接收缓冲区预分配

#### 3.1.2 TcpAcceptor

**职责**: TCP 服务器监听器，接受连接

**核心功能**:
1. **异步接受**
   - 支持无限并发连接
   - 端口复用 (SO_REUSEADDR)
   - backlog 配置

2. **地址配置**
   - 支持 IPv4/IPv6
   - 支持任意地址绑定
   - 支持指定接口绑定

#### 3.1.3 UdpSocket (可选扩展)

**职责**: 封装 UDP Socket，支持无连接通信

**核心功能**:
1. **异步发送**
   - 单播/广播/多播
   - 可靠性选项

2. **异步接收**
   - 从哪个 endpoint 接收
   - 数据包完整性检查

---

### 3.2 Channel 层设计

#### 3.2.1 Channel

**职责**: 高层通信通道，整合 Socket 和 Codec，提供消息级别接口

**核心功能**:
1. **生命周期管理**
   - 连接建立/断开回调
   - 心跳检测（可选）
   - 超时自动重连（可选）

2. **消息收发**
   - 自动编解码
   - 消息队列管理
   - 流量控制

3. **回调机制**
   - on_connect: 连接建立时
   - on_message: 收到消息时
   - on_close: 连接关闭时
   - on_error: 发生错误时

4. **线程安全**
   - send 操作线程安全
   - 回调在 io_context 线程执行

**状态机**:
```
  [Disconnected]
        │
        │ async_connect
        ▼
     [Connecting]
        │
        │ connect success
        ▼
   [Connected]
        │
        │ send/receive
        ▼
   [Active]
        │
        │ close / error
        ▼
  [Disconnected]
```

#### 3.2.2 ChannelFactory (可选)

**职责**: 简化 Channel 创建

```cpp
class ChannelFactory {
public:
    // 创建客户端 Channel
    static std::shared_ptr<Channel> create_client(
        IoContext& io,
        const std::string& host,
        uint16_t port,
        std::shared_ptr<Codec> codec
    );

    // 创建服务器 Channel（通过 accept 回调）
    static void create_server(
        IoContext& io,
        uint16_t port,
        std::shared_ptr<Codec> codec,
        std::function<void(std::shared_ptr<Channel>)> on_accept
    );
};
```

---

### 3.3 Codec 层设计

#### 3.3.1 Codec 接口

```cpp
class Codec {
public:
    virtual ~Codec() = default;

    // 编码：将应用层数据编码为网络字节流
    virtual ByteBuffer encode(const ByteBuffer& data) = 0;

    // 解码：从网络字节流提取完整消息
    // 返回: vector<完整消息>
    // buffer 会从前面移除已解码的字节
    virtual std::vector<ByteBuffer> decode(ByteBuffer& buffer) = 0;

    // 重置解码器状态
    virtual void reset() {}
};
```

#### 3.3.2 LengthPrefixCodec

**格式**: `[4字节长度(大端序)][数据体]`

**示例**:
```
数据: {0x01, 0x02, 0x03}
编码: {0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03}
```

**特性**:
- 支持二进制数据
- 精确的边界
- 简单高效

**限制**:
- 最大消息长度限制（防止 DoS）
- 不支持消息流式编码

#### 3.3.3 DelimiterCodec

**格式**: `[数据][分隔符]`

**分隔符选项**:
- `\n` - 换行符
- `\r\n` - Windows 换行
- 自定义字节

**特性**:
- 适合文本协议（如 HTTP、Redis）
- 支持流式编码
- 简单易读

**限制**:
- 分隔符不能出现在数据中
- 需要转义机制（可选）

#### 3.3.4 LineCodec (分隔符特例)

简化版的 DelimiterCodec，专门用于 `\n` 分隔。

---

## 4. 与其他层的交互

### 4.1 与 IoContext 层交互

**依赖关系**:
- Channel → TcpSocket → IoContext
- 所有异步操作通过 IoContext 调度

**线程模型**:
- 所有回调在 IoContext 线程执行
- 跨线程操作通过 io_context::post

### 4.2 与 Log 层交互

**日志记录**:
```cpp
// Socket 层日志
LOG_ERROR("Socket connect failed: {}", ec.message());

// Channel 层日志
LOG_INFO("Channel connected: {}:{}", endpoint.address(), endpoint.port());
LOG_DEBUG("Received message, size: {}", message.size());
```

### 4.3 与上层应用交互

**应用层接口**:
```cpp
// 1. 创建 Channel
auto channel = ChannelFactory::create_client(io, "127.0.0.1", 8080, codec);

// 2. 设置回调
channel->set_connect_callback([]() {
    LOG_INFO("Connected to server");
});

channel->set_message_callback([](const ByteBuffer& msg) {
    // 处理消息
    process_message(msg);
});

channel->set_close_callback([]() {
    LOG_WARN("Connection closed");
});

// 3. 启动
channel->start();

// 4. 发送消息
channel->send(data);
```

---

## 5. 错误处理机制

### 5.1 错误分类

#### 5.1.1 Socket 错误
```cpp
enum class SocketError {
    Success = 0,
    ConnectionRefused,      // 连接被拒绝
    ConnectionTimeout,     // 连接超时
    ConnectionReset,       // 连接被重置
    Disconnected,         // 已断开
    SendFailed,          // 发送失败
    ReceiveFailed,       // 接收失败
    InvalidEndpoint,      // 无效端点
    AlreadyConnected,     // 已连接
    NotConnected,        // 未连接
    BufferOverflow,      // 缓冲区溢出
    Unknown
};
```

#### 5.1.2 Channel 错误
```cpp
enum class ChannelError {
    Success = 0,
    CodecError,          // 编解码错误
    SendQueueFull,      // 发送队列满
    MessageTooLarge,    // 消息过大
    InvalidMessage,      // 无效消息
    ConnectionLost,     // 连接丢失
    ShutdownInProgress   // 正在关闭
};
```

#### 5.1.3 Codec 错误
```cpp
enum class CodecError {
    Success = 0,
    InvalidFormat,       // 格式错误
    ChecksumError,      // 校验和错误（可选）
    MessageTooLong,     // 消息过长
    MalformedData,      // 数据损坏
    IncompleteMessage,   // 消息不完整
    Unknown
};
```

### 5.2 错误处理策略

#### 5.2.1 Socket 层
1. **连接失败**: 返回错误码，由上层决定是否重试
2. **发送失败**: 记录日志，触发 close 回调
3. **接收失败**: 触发 close 回调，清理资源
4. **异常操作**: 抛出 std::runtime_error

#### 5.2.2 Channel 层
1. **编解码错误**: 记录日志，断开连接
2. **发送队列满**: 阻塞或丢弃（可配置）
3. **消息过大**: 记录日志，拒绝接收
4. **连接丢失**: 重试或通知上层（可配置）

#### 5.2.3 Codec 层
1. **格式错误**: 返回部分解码结果，上层决定如何处理
2. **数据不完整**: 等待更多数据
3. **数据损坏**: 抛出异常

### 5.3 错误恢复

#### 5.3.1 自动重连
```cpp
class Channel {
public:
    // 启用自动重连
    void enable_auto_reconnect(uint32_t max_retries, uint32_t interval_ms);

    // 禁用自动重连
    void disable_auto_reconnect();
};
```

#### 5.3.2 心跳检测
```cpp
class Channel {
public:
    // 设置心跳参数
    void set_heartbeat(uint32_t interval_ms, uint32_t timeout_ms);
};
```

---

## 6. 性能优化

### 6.1 内存优化

#### 6.1.1 缓冲区池
```cpp
class BufferPool {
public:
    ByteBuffer acquire(size_t size);
    void release(ByteBuffer&& buffer);

private:
    std::mutex mutex_;
    std::queue<ByteBuffer> free_buffers_;
};
```

**优化效果**:
- 减少 50%+ 的内存分配
- 提高发送性能 30%+

#### 6.1.2 零拷贝发送
- 使用 `const_buffer_view` 避免数据拷贝
- 支持 scatter-gather I/O

### 6.2 批量操作

#### 6.2.1 批量发送
```cpp
class Channel {
public:
    // 批量发送多个消息
    void send_batch(const std::vector<ByteBuffer>& messages);
};
```

**优化效果**:
- 减少 system call 次数
- 提高吞吐量 20%-50%

#### 6.2.2 批量接收
- 一次性接收多个消息
- 减少回调次数

### 6.3 I/O 优化

#### 6.3.1 TCP_NODELAY
```cpp
// 禁用 Nagle 算法，降低延迟
socket_.set_option(boost::asio::ip::tcp::no_delay(true));
```

#### 6.3.2 SO_RCVBUF/SO_SNDBUF
```cpp
// 调整缓冲区大小
socket_.set_option(boost::asio::socket_base::receive_buffer_size(64 * 1024));
socket_.set_option(boost::asio::socket_base::send_buffer_size(64 * 1024));
```

#### 6.3.3 SO_REUSEADDR
```cpp
// 允许端口复用
acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
```

### 6.4 编解码优化

#### 6.4.1 避免内存拷贝
- 使用引用传递
- 解码时移动而非拷贝

#### 6.4.2 预分配缓冲区
```cpp
class LengthPrefixCodec {
public:
    void set_buffer_capacity(size_t capacity) {
        decode_buffer_.reserve(capacity);
    }

private:
    ByteBuffer decode_buffer_;
};
```

---

## 7. 线程安全

### 7.1 线程安全保证

#### 7.1.1 TcpSocket
- ✅ `async_connect` - 线程安全
- ✅ `async_send` - 线程安全
- ✅ `async_receive` - 线程安全
- ✅ `close` - 线程安全
- ✅ `is_connected` - 线程安全

**实现**:
```cpp
void TcpSocket::async_send(const ByteBuffer& data, SendCallback callback) {
    // 通过 io_context::post 跨线程调用
    io_.post([this, data, callback]() {
        boost::asio::async_write(socket_, boost::asio::buffer(data),
            [this, callback](const error_code& ec, size_t) {
                callback(ec);
            });
    });
}
```

#### 7.1.2 Channel
- ✅ `send` - 线程安全
- ✅ `send_batch` - 线程安全
- ✅ `set_*_callback` - 线程安全
- ✅ `close` - 线程安全
- ✅ `is_open` - 线程安全

**实现**:
```cpp
void Channel::send(const ByteBuffer& data) {
    std::lock_guard<std::mutex> lock(send_mutex_);
    send_queue_.push(data);
    maybe_start_sending();
}
```

### 7.2 回调执行线程

**规则**: 所有回调在 IoContext 线程执行

**优点**:
- 避免跨线程同步
- 简化错误处理
- 统一事件循环

**应用层注意事项**:
```cpp
// 错误 ❌
channel->set_message_callback([](const ByteBuffer& msg) {
    // 如果这里执行耗时操作，会阻塞 IoContext 线程
    heavy_computation(msg);
});

// 正确 ✅
channel->set_message_callback([](const ByteBuffer& msg) {
    // 投递到工作线程
    worker_thread.post([msg]() {
        heavy_computation(msg);
    });
});
```

---

## 8. 使用示例

### 8.1 TCP 客户端

```cpp
#include <common/net/channel.h>
#include <common/net/codec.h>

using namespace rendu::net;

// 创建 IoContext
rendu::io::IoContext io(4);
std::thread([&io]() { io.run(); }).detach();

// 创建 Codec
auto codec = std::make_shared<LengthPrefixCodec>();

// 创建客户端 Channel
auto channel = ChannelFactory::create_client(
    io, "127.0.0.1", 8080, codec
);

// 设置回调
channel->set_connect_callback([]() {
    LOG_INFO("Connected to server");
});

channel->set_message_callback([](const ByteBuffer& msg) {
    LOG_INFO("Received message, size: {}", msg.size());
});

channel->set_close_callback([]() {
    LOG_WARN("Connection closed");
});

// 启动
channel->start();

// 发送消息
ByteBuffer data = {0x01, 0x02, 0x03};
channel->send(data);
```

### 8.2 TCP 服务器

```cpp
// 创建服务器
ChannelFactory::create_server(
    io, 8080, codec,
    [](std::shared_ptr<Channel> channel) {
        LOG_INFO("New client connected");

        channel->set_message_callback([channel](const ByteBuffer& msg) {
            LOG_INFO("Received from client: {}", msg.size());

            // Echo back
            channel->send(msg);
        });

        channel->set_close_callback([]() {
            LOG_INFO("Client disconnected");
        });

        channel->start();
    }
);
```

### 8.3 自定义 Codec

```cpp
class MyCodec : public Codec {
public:
    ByteBuffer encode(const ByteBuffer& data) override {
        // 自定义编码逻辑
        ByteBuffer encoded;
        // ... 编码 ...
        return encoded;
    }

    std::vector<ByteBuffer> decode(ByteBuffer& buffer) override {
        std::vector<ByteBuffer> messages;

        // 自定义解码逻辑
        while (has_complete_message(buffer)) {
            messages.push_back(extract_message(buffer));
        }

        return messages;
    }

private:
    bool has_complete_message(const ByteBuffer& buffer) {
        // 检查是否有完整消息
        return false;
    }

    ByteBuffer extract_message(ByteBuffer& buffer) {
        // 提取完整消息
        return {};
    }
};

// 使用自定义 Codec
auto codec = std::make_shared<MyCodec>();
auto channel = ChannelFactory::create_client(io, "127.0.0.1", 8080, codec);
```

---

## 9. 测试计划

### 9.1 单元测试

#### Socket 测试
- [x] 连接成功
- [ ] 连接失败（拒绝）
- [ ] 连接超时
- [x] 发送成功
- [x] 接收成功
- [ ] 断开连接
- [ ] 多线程 send

#### Channel 测试
- [ ] 连接回调
- [x] 消息回调
- [ ] 关闭回调
- [ ] 错误回调
- [x] 批量发送
- [ ] 线程安全

#### Codec 测试
- [x] LengthPrefixCodec 编码
- [x] LengthPrefixCodec 解码
- [ ] DelimiterCodec 编码
- [ ] DelimiterCodec 解码
- [x] 不完整消息处理
- [ ] 错误消息处理

### 9.2 集成测试

- [ ] 客户端-服务器通信
- [ ] 多客户端并发
- [ ] 大消息传输
- [ ] 长时间连接稳定性
- [ ] 异常断开恢复

### 9.3 性能测试

- [ ] 吞吐量测试（目标 ≥ 100MB/s）
- [ ] 延迟测试（目标 < 10ms）
- [ ] 并发连接测试（目标 1000+）
- [ ] 内存使用测试

---

## 10. 验收标准

### 功能验收
- [ ] Socket 连接建立/断开正常
- [ ] 数据收发无丢失
- [ ] 支持自定义编解码
- [ ] 错误处理正确
- [ ] 线程安全

### 性能验收
- [ ] 吞吐量 ≥ 100MB/s
- [ ] 延迟 < 10ms (局域网)
- [ ] 支持 1000+ 并发连接
- [ ] 内存使用 < 10MB/1000 连接

### 质量验收
- [ ] 单元测试覆盖率 ≥ 80%
- [ ] 无编译警告
- [ ] 通过静态分析
- [ ] 文档完整

---

## 11. 扩展计划

### 11.1 短期扩展
- [ ] UDP 支持
- [ ] TLS 支持
- [ ] HTTP/HTTPS 支持
- [ ] WebSocket 支持

### 11.2 长期扩展
- [ ] QUIC 协议
- [ ] HTTP/3
- [ ] RPC 框架集成
- [ ] 分布式追踪

---

## 12. 参考资料

- [Boost.Asio 官方文档](https://www.boost.org/doc/libs/release/libs/asio/)
- [TCP/IP 协议详解](https://www.rfc-editor.org/rfc/rfc793)
- [高性能服务器编程](https://github.com/chenshuo/muduo)
- [C++ 网络编程最佳实践](https://isocpp.org/)
