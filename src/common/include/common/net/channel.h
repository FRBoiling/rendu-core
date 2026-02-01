#pragma once

#include "common/define.h"
#include "common/net/socket.h"
#include "common/net/codec.h"
#include "common/net/buffer_view.h"
#include "common/log/logger.h"
#include "common/io/io_context.h"
#include <memory>
#include <functional>
#include <vector>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>

BEGIN_NAMESPACE_COMMON
namespace net {

/**
 * @brief Channel 错误码枚举
 */
enum class ChannelError {
    Success = 0,
    CodecError,          // 编解码错误
    SendQueueFull,      // 发送队列满
    MessageTooLarge,    // 消息过大
    InvalidMessage,      // 无效消息
    ConnectionLost,     // 连接丢失
    ShutdownInProgress   // 正在关闭
};

/**
 * @brief 通信通道类
 *
 * 整合 Socket 和 Codec，提供消息级别的通信接口
 * - 管理连接生命周期
 * - 自动编解码
 * - 提供应用层回调
 * - 线程安全的发送
 */
class Channel : public std::enable_shared_from_this<Channel> {
    friend class ChannelFactory;
public:
    using ConnectCallback = std::function<void()>;
    using CloseCallback = std::function<void(const boost::system::error_code&)>;
    using MessageCallback = std::function<void(const ByteBuffer&)>;
    using ErrorCallback = std::function<void(ChannelError, const std::string&)>;

    /**
     * @brief 构造函数
     * @param socket TcpSocket 对象
     * @param codec 编解码器
     */
    Channel(std::shared_ptr<TcpSocket> socket, std::shared_ptr<Codec> codec);
    
    ~Channel();

    // 禁止拷贝
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

    /**
     * @brief 启动 Channel（开始接收数据）
     */
    void start();

    /**
     * @brief 关闭 Channel
     */
    void close();

    /**
     * @brief 发送消息（线程安全）
     * @param data 要发送的消息数据
     */
    void send(const ByteBuffer& data);

    /**
     * @brief 零拷贝发送（线程安全）
     * @param view 缓冲区视图
     * @warning 调用者需确保 view 中的数据在发送完成前保持有效
     */
    void send_zero_copy(const BufferView& view);

    /**
     * @brief 批量发送多个消息（线程安全）
     * @param messages 消息列表
     */
    void send_batch(const std::vector<ByteBuffer>& messages);

    /**
     * @brief 零拷贝批量发送（线程安全）
     * @param views 缓冲区视图列表
     * @warning 调用者需确保 views 中的数据在发送完成前保持有效
     */
    void send_batch_zero_copy(const std::vector<BufferView>& views);

    /**
     * @brief 设置连接回调
     * @param callback 连接建立时调用
     */
    void set_connect_callback(ConnectCallback callback);

    /**
     * @brief 设置关闭回调
     * @param callback 连接关闭时调用
     */
    void set_close_callback(CloseCallback callback);

    /**
     * @brief 设置消息回调
     * @param callback 收到消息时调用
     */
    void set_message_callback(MessageCallback callback);

    /**
     * @brief 设置错误回调
     * @param callback 发生错误时调用
     */
    void set_error_callback(ErrorCallback callback);

    /**
     * @brief 检查 Channel 是否打开
     * @return 是否打开
     */
    bool is_open() const;

    /**
     * @brief 获取远程端点
     * @return 远程端点
     */
    boost::asio::ip::tcp::endpoint remote_endpoint() const;

    /**
     * @brief 获取本地端点
     * @return 本地端点
     */
    boost::asio::ip::tcp::endpoint local_endpoint() const;

    /**
     * @brief 设置接收缓冲区大小
     * @param size 缓冲区大小（字节）
     */
    void set_receive_buffer_size(size_t size);

private:
    /**
     * @brief 开始异步接收
     */
    void start_receive();

    /**
     * @brief 处理接收到的数据
     * @param ec 错误码
     * @param data 接收到的数据
     */
    void on_receive(const boost::system::error_code& ec, ByteBuffer data);

    /**
     * @brief 解码数据并调用回调
     * @param data 接收缓冲区
     */
    void decode_and_callback(ByteBuffer& data);

    /**
     * @brief 开始发送队列中的数据
     */
    void start_sending();

    /**
     * @brief 发送处理完成回调
     * @param ec 错误码
     * @param bytes_sent 发送字节数
     */
    void on_send_complete(const boost::system::error_code& ec, size_t bytes_sent);

    /**
     * @brief 触发错误回调
     * @param error 错误码
     * @param message 错误信息
     */
    void trigger_error(ChannelError error, const std::string& message);

    std::shared_ptr<TcpSocket> socket_;
    std::shared_ptr<Codec> codec_;

    // 接收缓冲区
    ByteBuffer receive_buffer_;
    size_t receive_buffer_size_;

    // 发送队列
    std::queue<ByteBuffer> send_queue_;
    std::mutex send_mutex_;
    bool sending_;

    // 回调
    ConnectCallback on_connect_;
    CloseCallback on_close_;
    MessageCallback on_message_;
    ErrorCallback on_error_;

    // 状态
    std::atomic<bool> started_;
    std::atomic<bool> closed_;
};

/**
 * @brief Channel 工厂类
 * 
 * 简化 Channel 的创建
 */
class ChannelFactory {
public:
    /**
     * @brief 创建客户端 Channel
     * @param io IoContext
     * @param host 主机名或 IP
     * @param port 端口号
     * @param codec 编解码器
     * @return Channel 对象（未连接）
     */
    static std::shared_ptr<Channel> create_client(
        io::IoContext& io,
        const std::string& host,
        uint16_t port,
        std::shared_ptr<Codec> codec
    );

    /**
     * @brief 创建服务器监听
     * @param io IoContext
     * @param port 监听端口
     * @param codec 编解码器
     * @param on_accept 接受连接回调
     */
    static void create_server(
        io::IoContext& io,
        uint16_t port,
        std::shared_ptr<Codec> codec,
        std::function<void(std::shared_ptr<Channel>)> on_accept
    );
};

} // namespace net
END_NAMESPACE_COMMON
