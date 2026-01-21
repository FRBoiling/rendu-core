# 阶段 4: Common 层 - 网络通信 (net)

## 目标
- 封装网络通信接口
- 提供统一的通信通道
- 支持自定义编解码器

---

## 文件结构

```
src/common/
├── include/common/net/
│   ├── socket.h
│   ├── channel.h
│   └── codec.h
└── src/net/
    ├── socket.cpp
    ├── channel.cpp
    └── codec.cpp
```

---

## 任务清单

### 1. Socket 封装 (socket.h/cpp)
- [ ] TCP Socket 封装
- [ ] UDP Socket 封装（可选）
- [ ] 异步连接/发送/接收
- [ ] 连接状态管理

### 2. 通信通道 (channel.h/cpp)
- [ ] 抽象通信接口
- [ ] 连接建立/断开回调
- [ ] 数据收发管理
- [ ] 错误处理

### 3. 编解码器 (codec.h/cpp)
- [ ] 编解码器接口
- [ ] 长度前缀编解码
- [ ] 分隔符编解码（可选）

---

## 头文件设计

### socket.h
```cpp
#pragma once

#include <memory>
#include <functional>
#include <boost/asio/ip/tcp.hpp>
#include <common/io/io_context.h>

namespace rendu::net {

using SocketCallback = std::function<void(const std::error_code& ec)>;

class TcpSocket {
public:
    explicit TcpSocket(IoContext& io);
    ~TcpSocket();

    // 异步连接
    void async_connect(const std::string& host, uint16_t port, SocketCallback callback);
    void async_connect(const boost::asio::ip::tcp::endpoint& endpoint, SocketCallback callback);

    // 异步发送
    void async_send(const std::vector<byte>& data, SocketCallback callback);

    // 异步接收
    void async_receive(size_t size, std::function<void(const std::error_code&, std::vector<byte>)> callback);

    // 关闭
    void close();

    // 状态
    bool is_connected() const;

    // 获取底层 socket
    boost::asio::ip::tcp::socket& native();

private:
    IoContext& io_;
    boost::asio::ip::tcp::socket socket_;
    bool connected_;
};

class TcpAcceptor {
public:
    explicit TcpAcceptor(IoContext& io, uint16_t port);
    ~TcpAcceptor();

    using AcceptCallback = std::function<void(std::shared_ptr<TcpSocket>, const std::error_code&)>;

    void async_accept(AcceptCallback callback);

    void close();

private:
    IoContext& io_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

} // namespace rendu::net
```

### channel.h
```cpp
#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <common/net/socket.h>
#include <common/net/codec.h>

namespace rendu::net {

class Channel {
public:
    using ConnectCallback = std::function<void()>;
    using CloseCallback = std::function<void()>;
    using MessageCallback = std::function<void(const std::vector<byte>&)>;

    Channel(std::shared_ptr<TcpSocket> socket, std::shared_ptr<Codec> codec);
    ~Channel();

    void start();
    void close();

    void send(const std::vector<byte>& data);

    void set_connect_callback(ConnectCallback cb);
    void set_close_callback(CloseCallback cb);
    void set_message_callback(MessageCallback cb);

    bool is_open() const;

private:
    void on_receive(const std::error_code& ec, std::vector<byte> data);
    void decode_and_callback(const std::vector<byte>& data);

    std::shared_ptr<TcpSocket> socket_;
    std::shared_ptr<Codec> codec_;
    std::vector<byte> buffer_;
    ConnectCallback on_connect_;
    CloseCallback on_close_;
    MessageCallback on_message_;
};

} // namespace rendu::net
```

### codec.h
```cpp
#pragma once

#include <vector>
#include <memory>

namespace rendu::net {

using ByteBuffer = std::vector<byte>;

class Codec {
public:
    virtual ~Codec() = default;

    // 编码
    virtual ByteBuffer encode(const ByteBuffer& data) = 0;

    // 解码（返回完整消息数）
    virtual std::vector<ByteBuffer> decode(ByteBuffer& buffer) = 0;
};

// 长度前缀编解码
class LengthPrefixCodec : public Codec {
public:
    explicit LengthPrefixCodec(uint32_t max_length = 1024 * 1024); // 1MB

    ByteBuffer encode(const ByteBuffer& data) override;
    std::vector<ByteBuffer> decode(ByteBuffer& buffer) override;

private:
    uint32_t max_length_;
};

// 分隔符编解码
class DelimiterCodec : public Codec {
public:
    explicit DelimiterCodec(byte delimiter);

    ByteBuffer encode(const ByteBuffer& data) override;
    std::vector<ByteBuffer> decode(ByteBuffer& buffer) override;

private:
    byte delimiter_;
};

} // namespace rendu::net
```

---

## 单元测试

### 测试文件
```
src/tests/common/net/
├── CMakeLists.txt
├── socket_test.cpp
├── channel_test.cpp
└── codec_test.cpp
```

### codec_test.cpp
```cpp
#include <catch2/catch_test_macros.hpp>
#include <common/net/codec.h>
#include <vector>

using namespace rendu::net;

TEST_CASE("LengthPrefixCodec encode", "[net][codec]") {
    LengthPrefixCodec codec;
    ByteBuffer data = {0x01, 0x02, 0x03};
    auto encoded = codec.encode(data);

    REQUIRE(encoded.size() == data.size() + 4); // 4字节长度前缀
}

TEST_CASE("LengthPrefixCodec decode", "[net][codec]") {
    LengthPrefixCodec codec;
    ByteBuffer data = {0x03, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03};

    auto messages = codec.decode(data);
    REQUIRE(messages.size() == 1);
    REQUIRE(messages[0].size() == 3);
}
```

---

## 验收标准

### 功能
- [ ] Socket 连接建立/断开正常
- [ ] 数据收发无丢失
- [ ] 支持自定义编解码
- [ ] 错误处理正确

### 性能
- [ ] 吞吐量 ≥ 100MB/s
- [ ] 延迟 < 10ms

### 线程安全
- [ ] 多线程 send 无数据竞争

---

## 下一步
完成本阶段后，进入 **阶段 5: Common 层 - 序列化 (ser)**
