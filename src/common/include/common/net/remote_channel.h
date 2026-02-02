#pragma once

#include "common/define.h"
#include "common/net/channel.h"
#include "common/net/socket.h"
#include "common/ser/protobuf_ser.h"
#include "common/log/logger.h"
#include "common/io/io_context.h"
#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <mutex>

// 前向声明 protobuf 消息类型
// 注意: 需要先编译 remote_message.proto 生成头文件
namespace rendu {
namespace remote {
    class RemoteActorRef;
    class RemoteMessage;
    class RemoteRequest;
    class RemoteResponse;
    class RemotePacket;
}
}

BEGIN_NAMESPACE_COMMON
namespace net {

/**
 * @brief 远程消息通道
 *
 * 封装网络通信，用于跨节点 Actor 消息传递
 * - 自动序列化/反序列化远程消息
 * - 支持 Ask/Tell 模式
 * - 自动重连机制
 */
class RemoteChannel : public std::enable_shared_from_this<RemoteChannel> {
public:
    using ConnectCallback = std::function<void()>;
    using DisconnectCallback = std::function<void()>;
    using MessageCallback = std::function<void(const std::shared_ptr<rendu::remote::RemoteMessage>&)>;
    using RequestCallback = std::function<void(const std::shared_ptr<rendu::remote::RemoteRequest>&)>;
    using ResponseCallback = std::function<void(const std::shared_ptr<rendu::remote::RemoteResponse>&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    /**
     * @brief 配置
     */
    struct Config {
        std::string node_id;               // 节点 ID
        uint32_t reconnect_interval{5000};   // 重连间隔（毫秒）
        uint32_t max_reconnect_attempts{0};   // 最大重连次数，0 表示无限
        bool auto_reconnect{true};           // 是否自动重连
    };

    /**
     * @brief 构造函数
     */
    explicit RemoteChannel(const Config& config, std::shared_ptr<io::IoContext> io_context);

    /**
     * @brief 析构函数
     */
    ~RemoteChannel();

    // 禁止拷贝
    RemoteChannel(const RemoteChannel&) = delete;
    RemoteChannel& operator=(const RemoteChannel&) = delete;

    /**
     * @brief 连接到远程节点
     * @param address 远程地址
     * @param port 远程端口
     */
    void connect(const std::string& address, uint16_t port);

    /**
     * @brief 断开连接
     */
    void disconnect();

    /**
     * @brief 发送 Tell 消息
     * @param message 消息
     */
    void send_message(const rendu::remote::RemoteMessage& message);

    /**
     * @brief 发送 Ask 请求
     * @param request 请求
     */
    void send_request(const rendu::remote::RemoteRequest& request);

    /**
     * @brief 发送 Ask 响应
     * @param response 响应
     */
    void send_response(const rendu::remote::RemoteResponse& response);

    /**
     * @brief 设置连接回调
     */
    void set_connect_callback(ConnectCallback callback);

    /**
     * @brief 设置断开回调
     */
    void set_disconnect_callback(DisconnectCallback callback);

    /**
     * @brief 设置消息回调
     */
    void set_message_callback(MessageCallback callback);

    /**
     * @brief 设置请求回调
     */
    void set_request_callback(RequestCallback callback);

    /**
     * @brief 设置响应回调
     */
    void set_response_callback(ResponseCallback callback);

    /**
     * @brief 设置错误回调
     */
    void set_error_callback(ErrorCallback callback);

    /**
     * @brief 是否已连接
     */
    bool is_connected() const;

private:
    /**
     * @brief 创建 TCP 连接
     */
    void create_connection();

    /**
     * @brief 处理连接建立
     */
    void on_connected();

    /**
     * @brief 处理连接断开
     */
    void on_disconnected();

    /**
     * @brief 开始重连
     */
    void start_reconnect();

    /**
     * @brief 处理接收到的数据
     */
    void on_data_received(const ByteBuffer& data);

    /**
     * @brief 解析远程消息包
     */
    void parse_remote_packet(const ByteBuffer& data);

    /**
     * @brief 发送原始数据
     */
    void send_raw(const ByteBuffer& data);

private:
    Config config_;
    std::shared_ptr<io::IoContext> io_context_;
    std::shared_ptr<TcpSocket> socket_;
    std::shared_ptr<Channel> channel_;

    // 回调
    ConnectCallback connect_callback_;
    DisconnectCallback disconnect_callback_;
    MessageCallback message_callback_;
    RequestCallback request_callback_;
    ResponseCallback response_callback_;
    ErrorCallback error_callback_;

    // 状态
    std::atomic<bool> connected_{false};
    std::atomic<bool> connecting_{false};
    std::atomic<uint32_t> reconnect_count_{0};
    std::atomic<bool> stopped_{false};

    // 连接信息
    std::string remote_address_;
    uint16_t remote_port_{0};

    // 互斥锁
    mutable std::mutex mutex_;
};

}  // namespace net
END_NAMESPACE_COMMON
