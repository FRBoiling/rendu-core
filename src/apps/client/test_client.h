#pragma once

#include "common/define.h"
#include "common/net/socket.h"
#include "common/net/codec.h"
#include "common/io/io_context.h"
#include "messages.pb.h"
#include <string>
#include <functional>
#include <memory>
#include <atomic>
#include <chrono>

BEGIN_NAMESPACE_COMMON
namespace net {

/**
 * @brief 测试客户端
 *
 * 用于服务器性能测试的简单客户端实现
 */
class TestClient {
public:
    /**
     * @brief 事件回调
     */
    using OnConnectedCallback = std::function<void()>;
    using OnMessageCallback = std::function<void(const protocol::ServerMessage&)>;
    using OnErrorCallback = std::function<void(const std::string&)>;

    /**
     * @brief 统计信息
     */
    struct Stats {
        std::atomic<uint64_t> messages_sent{0};
        std::atomic<uint64_t> messages_received{0};
        std::atomic<uint64_t> bytes_sent{0};
        std::atomic<uint64_t> bytes_received{0};
        std::chrono::steady_clock::time_point connect_time;
        std::chrono::steady_clock::time_point disconnect_time;
    };

    /**
     * @brief 构造函数
     * @param client_id 客户端 ID
     * @param io IoContext 引用
     */
    TestClient(int client_id, io::IoContext& io);

    /**
     * @brief 析构函数
     */
    ~TestClient();

    /**
     * @brief 连接到服务器
     * @param host 主机地址
     * @param port 端口号
     * @param callback 连接完成回调
     */
    void connect(const std::string& host, uint16_t port, OnConnectedCallback callback);

    /**
     * @brief 发送登录请求
     * @param username 用户名
     */
    void login(const std::string& username);

    /**
     * @brief 发送聊天消息
     * @param content 消息内容
     */
    void send_chat(const std::string& content);

    /**
     * @brief 开始持续发送聊天消息
     * @param interval_ms 发送间隔（毫秒）
     * @param content 消息内容
     */
    void start_chat_loop(int interval_ms, const std::string& content);

    /**
     * @brief 停止聊天循环
     */
    void stop_chat_loop();

    /**
     * @brief 断开连接
     */
    void disconnect();

    /**
     * @brief 设置消息回调
     * @param callback 消息回调
     */
    void set_message_callback(OnMessageCallback callback) {
        on_message_ = callback;
    }

    /**
     * @brief 设置错误回调
     * @param callback 错误回调
     */
    void set_error_callback(OnErrorCallback callback) {
        on_error_ = callback;
    }

    /**
     * @brief 获取统计信息
     * @return 统计信息引用
     */
    const Stats& stats() const { return stats_; }

    /**
     * @brief 获取客户端 ID
     * @return 客户端 ID
     */
    int client_id() const { return client_id_; }

    /**
     * @brief 检查是否已连接
     * @return 是否连接
     */
    bool is_connected() const;

    /**
     * @brief 计算运行时间（毫秒）
     * @return 运行时间
     */
    int64_t uptime_ms() const;

private:
    /**
     * @brief 发送原始数据
     * @param data 要发送的数据
     */
    void send_raw(const std::vector<byte>& data);

    /**
     * @brief 处理接收到的数据
     * @param data 接收到的数据
     */
    void on_data_received(const std::vector<byte>& data);

    /**
     * @brief 开始接收数据
     */
    void start_receive();

private:
    int client_id_;
    io::IoContext& io_;
    std::unique_ptr<TcpSocket> socket_;
    std::unique_ptr<LengthPrefixCodec> codec_;

    OnConnectedCallback on_connected_;
    OnMessageCallback on_message_;
    OnErrorCallback on_error_;

    Stats stats_;
    std::vector<byte> receive_buffer_;
    std::mutex buffer_mutex_;

    bool chat_loop_running_{false};
    std::chrono::milliseconds chat_interval_{0};
    std::string chat_content_;
};

} // namespace net
END_NAMESPACE_COMMON
