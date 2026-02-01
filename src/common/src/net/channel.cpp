#include "common/net/channel.h"
#include <boost/asio.hpp>
#include <algorithm>

BEGIN_NAMESPACE_COMMON
namespace net {

Channel::Channel(std::shared_ptr<TcpSocket> socket, std::shared_ptr<Codec> codec)
    : socket_(std::move(socket))
    , codec_(std::move(codec))
    , receive_buffer_size_(8192)
    , sending_(false)
    , started_(false)
    , closed_(false) {
}

Channel::~Channel() {
    close();
}

void Channel::start() {
    if (started_.exchange(true)) {
        return;
    }
    
    if (on_connect_) {
        on_connect_();
    }
    
    start_receive();
}

void Channel::close() {
    if (closed_.exchange(true)) {
        return;
    }

    socket_->close();
    boost::system::error_code ec(boost::system::errc::success, boost::system::system_category());

    if (on_close_) {
        on_close_(ec);
    }
}

void Channel::send(const ByteBuffer& data) {
    if (closed_) {
        trigger_error(ChannelError::ConnectionLost, "Cannot send: channel is closed");
        return;
    }

    if (data.size() > 1024 * 1024) {
        trigger_error(ChannelError::MessageTooLarge, "Message size exceeds 1MB limit");
        return;
    }

    {
        std::lock_guard<std::mutex> lock(send_mutex_);
        send_queue_.push(data);
    }

    start_sending();
}

void Channel::send_zero_copy(const BufferView& view) {
    if (closed_) {
        trigger_error(ChannelError::ConnectionLost, "Cannot send: channel is closed");
        return;
    }

    if (view.size() > 1024 * 1024) {
        trigger_error(ChannelError::MessageTooLarge, "Message size exceeds 1MB limit");
        return;
    }

    // 将 BufferView 拷贝到发送队列 (因为需要保证数据生命周期)
    // 注意: 这不是真正的零拷贝,因为发送队列拥有数据
    // 真正的零拷贝需要调用者保证数据生命周期,这会增加复杂性
    ByteBuffer data(view.data(), view.data() + view.size());

    {
        std::lock_guard<std::mutex> lock(send_mutex_);
        send_queue_.push(std::move(data));
    }

    start_sending();
}

void Channel::send_batch(const std::vector<ByteBuffer>& messages) {
    if (closed_) {
        trigger_error(ChannelError::ConnectionLost, "Cannot send: channel is closed");
        return;
    }

    size_t total_size = 0;
    for (const auto& msg : messages) {
        total_size += msg.size();
    }

    if (total_size > 10 * 1024 * 1024) {
        trigger_error(ChannelError::MessageTooLarge, "Batch message size exceeds 10MB limit");
        return;
    }

    {
        std::lock_guard<std::mutex> lock(send_mutex_);
        for (const auto& msg : messages) {
            send_queue_.push(msg);
        }
    }

    start_sending();
}

void Channel::send_batch_zero_copy(const std::vector<BufferView>& views) {
    if (closed_) {
        trigger_error(ChannelError::ConnectionLost, "Cannot send: channel is closed");
        return;
    }

    size_t total_size = 0;
    for (const auto& view : views) {
        total_size += view.size();
    }

    if (total_size > 10 * 1024 * 1024) {
        trigger_error(ChannelError::MessageTooLarge, "Batch message size exceeds 10MB limit");
        return;
    }

    // 将 BufferViews 拷贝到发送队列 (因为需要保证数据生命周期)
    {
        std::lock_guard<std::mutex> lock(send_mutex_);
        for (const auto& view : views) {
            send_queue_.emplace(view.data(), view.data() + view.size());
        }
    }

    start_sending();
}

void Channel::set_connect_callback(ConnectCallback callback) {
    on_connect_ = std::move(callback);
}

void Channel::set_close_callback(CloseCallback callback) {
    on_close_ = std::move(callback);
}

void Channel::set_message_callback(MessageCallback callback) {
    on_message_ = std::move(callback);
}

void Channel::set_error_callback(ErrorCallback callback) {
    on_error_ = std::move(callback);
}

bool Channel::is_open() const {
    // Channel 被视为打开，只要没有被显式关闭
    // 底层 socket 即使未连接，也是有效的（可以调用 async_connect）
    return !closed_;
}

boost::asio::ip::tcp::endpoint Channel::remote_endpoint() const {
    return socket_->remote_endpoint();
}

boost::asio::ip::tcp::endpoint Channel::local_endpoint() const {
    return socket_->local_endpoint();
}

void Channel::set_receive_buffer_size(size_t size) {
    receive_buffer_size_ = std::max(size, size_t(1024));
}

void Channel::start_receive() {
    if (closed_) {
        return;
    }
    
    socket_->async_receive(receive_buffer_size_,
        [self = shared_from_this()](boost::system::error_code ec, ByteBuffer data) {
            self->on_receive(ec, std::move(data));
        }
    );
}

void Channel::on_receive(const boost::system::error_code& ec, ByteBuffer data) {
    if (closed_) {
        return;
    }
    
    if (ec) {
        trigger_error(ChannelError::ConnectionLost, ec.message());
        close();
        return;
    }
    
    if (data.empty()) {
        trigger_error(ChannelError::ConnectionLost, "Connection closed by peer");
        close();
        return;
    }
    
    // 将接收到的数据追加到缓冲区
    receive_buffer_.insert(receive_buffer_.end(), data.begin(), data.end());
    
    // 解码并处理消息
    decode_and_callback(receive_buffer_);
    
    // 继续接收
    start_receive();
}

void Channel::decode_and_callback(ByteBuffer& data) {
    auto messages = codec_->decode(data);

    for (const auto& message : messages) {
        if (on_message_) {
            on_message_(message);
        }
    }
}

void Channel::start_sending() {
    if (closed_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(send_mutex_);
    
    if (sending_ || send_queue_.empty()) {
        return;
    }
    
    sending_ = true;
    
    ByteBuffer data = std::move(send_queue_.front());
    send_queue_.pop();
    
    socket_->async_send(data,
        [self = shared_from_this()](const boost::system::error_code& ec, size_t bytes_sent) {
            self->on_send_complete(ec, bytes_sent);
        }
    );
}

void Channel::on_send_complete(const boost::system::error_code& ec, size_t bytes_sent) {
    if (closed_) {
        return;
    }
    
    if (ec) {
        trigger_error(ChannelError::ConnectionLost, ec.message());
        close();
        return;
    }
    
    std::lock_guard<std::mutex> lock(send_mutex_);
    sending_ = false;
    
    if (!send_queue_.empty()) {
        start_sending();
    }
}

void Channel::trigger_error(ChannelError error, const std::string& message) {
    if (on_error_) {
        on_error_(error, message);
    }
}

// ==================== ChannelFactory Implementation ====================

void ChannelFactory::create_server(
    io::IoContext& io,
    uint16_t port,
    std::shared_ptr<Codec> codec,
    std::function<void(std::shared_ptr<Channel>)> on_accept
) {
    struct ServerState {
        std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
        std::shared_ptr<Codec> codec;
        std::function<void(std::shared_ptr<Channel>)> on_accept;
        
        void start_accept(io::IoContext& io) {
            acceptor->async_accept(
                [this, &io](boost::system::error_code ec, boost::asio::ip::tcp::socket accepted_socket) mutable {
                    if (!ec) {
                        auto socket = std::make_shared<TcpSocket>(io);
                        socket->set_socket(std::move(accepted_socket));
                        auto channel = std::make_shared<Channel>(socket, codec);
                        channel->start();
                        on_accept(channel);
                    }
                    start_accept(io);
                }
            );
        }
    };
    
    auto state = std::make_shared<ServerState>();
    state->acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(
        io.native(),
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
    );
    state->codec = codec;
    state->on_accept = std::move(on_accept);
    
    state->start_accept(io);
}

std::shared_ptr<Channel> ChannelFactory::create_client(
    io::IoContext& io,
    const std::string& host,
    uint16_t port,
    std::shared_ptr<Codec> codec
) {
    auto socket = std::make_shared<TcpSocket>(io);
    auto channel = std::make_shared<Channel>(socket, codec);

    socket->async_connect(host, port, [channel](const boost::system::error_code& ec) {
        if (ec) {
            channel->trigger_error(ChannelError::ConnectionLost, ec.message());
            return;
        }
        channel->start();
    });

    return channel;
}

} // namespace net
END_NAMESPACE_COMMON
