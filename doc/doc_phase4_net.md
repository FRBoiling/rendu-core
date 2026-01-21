# 阶段 4: Common 层 - 网络通信 (net)

## 目标
封装网络通信接口，提供统一的通信通道。

---

## 任务清单

### 1. socket.h
**文件路径**: `src/common/include/common/net/socket.h`

**职责**:
- Socket 封装（TCP/UDP）
- 异步连接/接受
- 异步读写

**核心接口**:
```cpp
class Socket {
public:
    // 异步连接
    template<typename Callback>
    void AsyncConnect(const std::string& host, uint16_t port, Callback&& cb);

    // 异步接受连接
    template<typename Callback>
    void AsyncAccept(Callback&& cb);

    // 异步读取
    template<typename Callback>
    void AsyncRead(std::shared_ptr<Buffer> buffer, Callback&& cb);

    // 异步写入
    template<typename Callback>
    void AsyncWrite(std::shared_ptr<Buffer> buffer, Callback&& cb);

    // 关闭连接
    void Close();

    // 获取远程地址
    std::string GetRemoteAddress() const;

    // 获取远程端口
    uint16_t GetRemotePort() const;
};
```

---

### 2. channel.h
**文件路径**: `src/common/include/common/net/channel.h`

**职责**:
- 通信通道抽象
- 管理连接状态
- 处理发送/接收队列

**核心接口**:
```cpp
class Channel {
public:
    using MessageCallback = std::function<void(const Message&)>;
    using ConnectionCallback = std::function<void()>;

    // 设置消息回调
    void SetMessageCallback(MessageCallback cb);

    // 设置连接回调
    void SetConnectionCallback(ConnectionCallback cb);

    // 发送消息
    void Send(const Message& msg);

    // 关闭通道
    void Close();

    // 是否已连接
    bool IsConnected() const;
};
```

---

### 3. codec.h
**文件路径**: `src/common/include/common/net/codec.h`

**职责**:
- 编解码器接口
- 处理消息序列化/反序列化

**核心接口**:
```cpp
class ICodec {
public:
    virtual ~ICodec() = default;

    // 编码
    virtual std::vector<std::byte> Encode(const Message& msg) = 0;

    // 解码
    virtual std::optional<Message> Decode(const Buffer& buffer) = 0;
};
```

**内置实现**:
- `LengthHeaderCodec` - 长度前缀编码
- `DelimiterCodec` - 分隔符编码

---

## 设计要点

### 1. 异步模型
- 基于 io_context 的异步 I/O
- 回调方式处理完成事件
- 支持 C++20 协程（可选）

### 2. 连接管理
- 自动重连机制
- 心跳检测
- 超时处理

### 3. 缓冲区管理
- 使用 Buffer 类管理内存
- 零拷贝优化
- 预分配池

### 4. 错误处理
- 统一错误码
- 断线重连
- 优雅关闭

---

## 验收标准

- [ ] TCP/UDP 连接建立正常
- [ ] 异步读写性能符合预期
- [ ] 数据收发无丢失
- [ ] 连接断开处理正确
- [ ] 重连机制工作正常
- [ ] 性能测试通过
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 2 (io) - io_context
- 阶段 3 (log) - 日志记录
- 阶段 5 (ser) - 序列化
- 3rdparty::boost - Boost.Asio
