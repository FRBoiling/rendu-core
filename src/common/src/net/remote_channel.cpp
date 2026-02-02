#include "common/net/remote_channel.h"
#include "common/ser/protobuf_ser.h"
#include "common/log/logger.h"

// 包含生成的 protobuf 头文件
#include "remote_message.pb.h"
#include "common/config/config.h"

using namespace COMMON_NAMESPACE;
using namespace COMMON_NAMESPACE::net;
using namespace COMMON_NAMESPACE::io;
using namespace COMMON_NAMESPACE::config;

RemoteChannel::RemoteChannel(const Config& config, std::shared_ptr<io::IoContext> io_context)
    : config_(config)
      , io_context_(io_context)
{
}

RemoteChannel::~RemoteChannel()
{
    disconnect();
    stopped_.store(true);
}

void RemoteChannel::connect(const std::string& address, uint16_t port)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (stopped_.load())
    {
        RENDU_LOG_WARN("RemoteChannel is stopped, cannot connect to {}:{}", address, port);
        return;
    }

    if (connected_.load())
    {
        RENDU_LOG_INFO("Already connected to {}:{}", address, port);
        return;
    }

    remote_address_ = address;
    remote_port_ = port;

    RENDU_LOG_INFO("Connecting to remote node {}:{}...", address, port);
    create_connection();
}

void RemoteChannel::disconnect()
{
    std::lock_guard<std::mutex> lock(mutex_);

    RENDU_LOG_INFO("Disconnecting from {}:{}", remote_address_, remote_port_);

    if (channel_)
    {
        channel_->close();
        channel_.reset();
    }

    if (socket_)
    {
        socket_->close();
        socket_.reset();
    }

    connected_.store(false);
    connecting_.store(false);
}

void RemoteChannel::send_message(const rendu::remote::RemoteMessage& message)
{
    if (!is_connected())
    {
        RENDU_LOG_ERROR("Cannot send message: not connected to {}:{}", remote_address_, remote_port_);
        if (error_callback_)
        {
            error_callback_("Not connected to remote node");
        }
        return;
    }

    // 创建 RemotePacket 封装
    rendu::remote::RemotePacket packet;
    packet.mutable_message()->CopyFrom(message);

    // 序列化为 protobuf
    auto result = ser::ProtobufSerializer::serialize_message(packet);
    if (std::holds_alternative<Error>(result))
    {
        RENDU_LOG_ERROR("Failed to serialize message: {}", std::get<Error>(result).message());
        if (error_callback_)
        {
            error_callback_("Failed to serialize message");
        }
        return;
    }

    // 发送
    send_raw(std::get<ByteBuffer>(result));
    RENDU_LOG_DEBUG("Sent message from {} to {}",
                    message.sender().path(), message.receiver().path());
}

void RemoteChannel::send_request(const rendu::remote::RemoteRequest& request)
{
    if (!is_connected())
    {
        RENDU_LOG_ERROR("Cannot send request: not connected to {}:{}", remote_address_, remote_port_);
        if (error_callback_)
        {
            error_callback_("Not connected to remote node");
        }
        return;
    }

    // 创建 RemotePacket 封装
    rendu::remote::RemotePacket packet;
    packet.mutable_request()->CopyFrom(request);

    // 序列化为 protobuf
    auto result = ser::ProtobufSerializer::serialize_message(packet);
    if (std::holds_alternative<Error>(result))
    {
        RENDU_LOG_ERROR("Failed to serialize request: {}", std::get<Error>(result).message());
        if (error_callback_)
        {
            error_callback_("Failed to serialize request");
        }
        return;
    }

    // 发送
    send_raw(std::get<ByteBuffer>(result));
    RENDU_LOG_DEBUG("Sent request {} from {} to {}",
                    request.request_id(), request.sender().path(), request.receiver().path());
}

void RemoteChannel::send_response(const rendu::remote::RemoteResponse& response)
{
    if (!is_connected())
    {
        RENDU_LOG_ERROR("Cannot send response: not connected to {}:{}", remote_address_, remote_port_);
        if (error_callback_)
        {
            error_callback_("Not connected to remote node");
        }
        return;
    }

    // 创建 RemotePacket 封装
    rendu::remote::RemotePacket packet;
    packet.mutable_response()->CopyFrom(response);

    // 序列化为 protobuf
    auto result = ser::ProtobufSerializer::serialize_message(packet);
    if (std::holds_alternative<Error>(result))
    {
        RENDU_LOG_ERROR("Failed to serialize response: {}", std::get<Error>(result).message());
        if (error_callback_)
        {
            error_callback_("Failed to serialize response");
        }
        return;
    }

    // 发送
    send_raw(std::get<ByteBuffer>(result));
    RENDU_LOG_DEBUG("Sent response {} from {} to {}",
                    response.request_id(), response.sender().path(), response.receiver().path());
}

void RemoteChannel::set_connect_callback(ConnectCallback callback)
{
    connect_callback_ = callback;
}

void RemoteChannel::set_disconnect_callback(DisconnectCallback callback)
{
    disconnect_callback_ = callback;
}

void RemoteChannel::set_message_callback(MessageCallback callback)
{
    message_callback_ = callback;
}

void RemoteChannel::set_request_callback(RequestCallback callback)
{
    request_callback_ = callback;
}

void RemoteChannel::set_response_callback(ResponseCallback callback)
{
    response_callback_ = callback;
}

void RemoteChannel::set_error_callback(ErrorCallback callback)
{
    error_callback_ = callback;
}

bool RemoteChannel::is_connected() const
{
    return connected_.load() && channel_ && channel_->is_open();
}

void RemoteChannel::create_connection()
{
    // 使用 LengthPrefixCodec 处理 protobuf 消息
    auto codec = std::make_shared<LengthPrefixCodec>();

    // 创建 TcpSocket
    socket_ = std::make_shared<TcpSocket>(*io_context_);

    // 设置 TCP 优化参数
    TcpOptimization tcp_opt;
    tcp_opt.no_delay = true; // 低延迟
    tcp_opt.keepalive = true;
    tcp_opt.keepalive_idle = 60;
    tcp_opt.keepalive_interval = 10;
    tcp_opt.keepalive_count = 3;
    apply_tcp_optimizations(socket_->native_socket(), tcp_opt);

    // 异步连接到远程节点
    socket_->async_connect(remote_address_, remote_port_,
        [this, codec](const boost::system::error_code& ec)
        {
            if (ec)
            {
                RENDU_LOG_ERROR("Failed to connect to {}:{}: {}", remote_address_, remote_port_,
                                ec.message());

                if (error_callback_)
                {
                    error_callback_("Failed to connect: " + ec.message());
                }

                // 触发断开处理
                on_disconnected();
                return;
            }

            RENDU_LOG_DEBUG("TCP socket connected to {}:{}", remote_address_, remote_port_);

            // 创建 Channel
            channel_ = std::make_shared<Channel>(socket_, codec);

            // 设置回调
            channel_->set_connect_callback([this]()
            {
                on_connected();
            });

            channel_->set_close_callback([this](const boost::system::error_code& ec)
            {
                if (ec)
                {
                    RENDU_LOG_WARN("Channel closed with error: {}", ec.message());
                    if (error_callback_)
                    {
                        error_callback_("Channel closed: " + ec.message());
                    }
                }
                on_disconnected();
            });

            channel_->set_message_callback([this](const ByteBuffer& data)
            {
                on_data_received(data);
            });

            channel_->set_error_callback([this](ChannelError err, const std::string& msg)
            {
                RENDU_LOG_ERROR("Channel error: {} - {}", static_cast<int>(err), msg);
                if (error_callback_)
                {
                    error_callback_("Channel error: " + msg);
                }
            });

            // 启动 Channel
            channel_->start();
        });
}

void RemoteChannel::on_connected()
{
    connected_.store(true);
    connecting_.store(false);
    reconnect_count_.store(0);

    RENDU_LOG_INFO("Remote channel connected to {}:{}", remote_address_, remote_port_);

    if (connect_callback_)
    {
        connect_callback_();
    }
}

void RemoteChannel::on_disconnected()
{
    connected_.store(false);
    connecting_.store(false);

    RENDU_LOG_INFO("Remote channel disconnected from {}:{}", remote_address_, remote_port_);

    if (disconnect_callback_)
    {
        disconnect_callback_();
    }

    // 自动重连
    if (config_.auto_reconnect && !stopped_.load())
    {
        start_reconnect();
    }
}

void RemoteChannel::start_reconnect()
{
    uint32_t max_attempts = config_.max_reconnect_attempts;
    uint32_t current_attempts = reconnect_count_.load();

    if (max_attempts > 0 && current_attempts >= max_attempts)
    {
        RENDU_LOG_WARN("Max reconnect attempts ({}) reached for {}:{}",
                       max_attempts, remote_address_, remote_port_);
        return;
    }

    reconnect_count_.fetch_add(1);

    RENDU_LOG_INFO("Reconnecting to {}:{} (attempt {})...",
                   remote_address_, remote_port_, current_attempts + 1);

    // 使用 IoContext 的 timer 延迟重连
    // TODO: 实现延迟重连（需要 timer 支持）
    // 目前直接重连
    create_connection();
}

void RemoteChannel::on_data_received(const ByteBuffer& data)
{
    RENDU_LOG_DEBUG("Received {} bytes from {}:{}", data.size(), remote_address_, remote_port_);

    // 反序列化 RemotePacket
    auto result = ser::ProtobufSerializer::deserialize_message<rendu::remote::RemotePacket>(data);

    if (std::holds_alternative<Error>(result))
    {
        RENDU_LOG_ERROR("Failed to deserialize packet: {}", std::get<Error>(result).message());
        if (error_callback_)
        {
            error_callback_("Failed to deserialize packet");
        }
        return;
    }

    const auto& packet = std::get<rendu::remote::RemotePacket>(result);

    // 根据包类型分发
    if (packet.has_message())
    {
        RENDU_LOG_DEBUG("Received RemoteMessage from {} to {}",
                        packet.message().sender().path(),
                        packet.message().receiver().path());
        if (message_callback_)
        {
            message_callback_(std::make_shared<rendu::remote::RemoteMessage>(packet.message()));
        }
    }
    else if (packet.has_request())
    {
        RENDU_LOG_DEBUG("Received RemoteRequest {} from {} to {}",
                        packet.request().request_id(),
                        packet.request().sender().path(),
                        packet.request().receiver().path());
        if (request_callback_)
        {
            request_callback_(std::make_shared<rendu::remote::RemoteRequest>(packet.request()));
        }
    }
    else if (packet.has_response())
    {
        RENDU_LOG_DEBUG("Received RemoteResponse {} from {} to {}",
                        packet.response().request_id(),
                        packet.response().sender().path(),
                        packet.response().receiver().path());
        if (response_callback_)
        {
            response_callback_(std::make_shared<rendu::remote::RemoteResponse>(packet.response()));
        }
    }
    else
    {
        RENDU_LOG_WARN("Received RemotePacket with unknown type");
    }
}

void RemoteChannel::send_raw(const ByteBuffer& data)
{
    if (channel_)
    {
        channel_->send(data);
    }
    else
    {
        RENDU_LOG_ERROR("Cannot send: channel is null");
        if (error_callback_)
        {
            error_callback_("Channel is null");
        }
    }
}
