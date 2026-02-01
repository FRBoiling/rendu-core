#include "common/net/connection_pool.h"

BEGIN_NAMESPACE_COMMON
namespace net {

ConnectionPool::ConnectionPool(
    io::IoContext& io,
    const std::string& host,
    uint16_t port,
    const ConnectionPoolConfig& config
)
    : io_(io)
    , host_(host)
    , port_(port)
    , config_(config)
{
    RENDU_LOG_INFO("ConnectionPool created for {}:{}", host, port);
}

ConnectionPool::~ConnectionPool() {
    close_all();
}

std::future<std::shared_ptr<Channel>> ConnectionPool::acquire() {
    auto promise = std::make_shared<std::promise<std::shared_ptr<Channel>>>();
    auto future = promise->get_future();

    std::lock_guard<std::mutex> lock(mutex_);

    // 检查是否有空闲连接
    while (!idle_connections_.empty()) {
        auto channel = idle_connections_.front();
        idle_connections_.pop();

        // 检查连接是否仍然有效
        if (is_connection_valid(channel)) {
            ++active_count_;
            promise->set_value(channel);
            RENDU_LOG_DEBUG("Reused idle connection for {}:{}", host_, port_);
            return future;
        } else {
            RENDU_LOG_DEBUG("Discarded invalid connection for {}:{}", host_, port_);
            continue;
        }
    }

    // 没有可用连接,检查是否可以创建新连接
    size_t current_total = active_count_ + idle_connections_.size();
    if (current_total < config_.max_connections) {
        // 异步创建新连接
        io_.post([this, promise]() {
            try {
                auto channel = create_connection();
                if (channel) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    ++active_count_;
                    ++total_created_;
                    promise->set_value(channel);
                    RENDU_LOG_DEBUG("Created new connection for {}:{}", host_, port_);
                } else {
                    promise->set_value(nullptr);
                    RENDU_LOG_ERROR("Failed to create connection for {}:{}", host_, port_);
                }
            } catch (const std::exception& e) {
                promise->set_value(nullptr);
                RENDU_LOG_ERROR("Exception creating connection for {}:{}: {}", host_, port_, e.what());
            }
        });
    } else {
        // 已达到最大连接数,将请求加入等待队列
        pending_requests_.push(promise);
        RENDU_LOG_DEBUG("Connection pool full, request queued for {}:{}", host_, port_);
    }

    return future;
}

void ConnectionPool::release(std::shared_ptr<Channel> channel) {
    if (!channel) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    // 减少活跃连接数
    --active_count_;

    // 检查连接是否仍然有效
    if (!is_connection_valid(channel)) {
        RENDU_LOG_DEBUG("Closed invalid connection for {}:{}", host_, port_);
        return;
    }

    // 检查是否有等待的请求
    if (!pending_requests_.empty()) {
        auto promise = std::move(pending_requests_.front());
        pending_requests_.pop();
        ++active_count_;
        promise->set_value(channel);
        RENDU_LOG_DEBUG("Reused connection for pending request {}:{}", host_, port_);
    } else {
        // 放回空闲队列
        idle_connections_.push(channel);
        cv_.notify_one();
        RENDU_LOG_DEBUG("Returned connection to idle pool {}:{}", host_, port_);
    }
}

void ConnectionPool::cleanup_idle_connections() {
    std::lock_guard<std::mutex> lock(mutex_);

    std::queue<std::shared_ptr<Channel>> cleaned;
    size_t cleaned_count = 0;

    // 清理已关闭的连接
    while (!idle_connections_.empty()) {
        auto channel = idle_connections_.front();
        idle_connections_.pop();

        if (is_connection_valid(channel)) {
            cleaned.push(channel);
        } else {
            ++cleaned_count;
        }
    }

    idle_connections_ = std::move(cleaned);

    if (cleaned_count > 0) {
        RENDU_LOG_INFO("Cleaned up {} idle connections for {}:{}", cleaned_count, host_, port_);
    }
}

void ConnectionPool::close_all() {
    std::lock_guard<std::mutex> lock(mutex_);

    // 关闭所有空闲连接
    size_t count = 0;
    while (!idle_connections_.empty()) {
        auto channel = idle_connections_.front();
        idle_connections_.pop();
        if (channel && channel->is_open()) {
            channel->close();
            ++count;
        }
    }

    // 取消所有等待的请求
    while (!pending_requests_.empty()) {
        auto promise = pending_requests_.front();
        pending_requests_.pop();
        promise->set_value(nullptr);
    }

    active_count_ = 0;

    RENDU_LOG_INFO("Closed {} connections for {}:{}", count, host_, port_);
}

ConnectionPool::Stats ConnectionPool::get_stats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return Stats{
        .idle_count = idle_connections_.size(),
        .active_count = active_count_.load(),
        .total_count = idle_connections_.size() + active_count_.load()
    };
}

std::shared_ptr<Channel> ConnectionPool::create_connection() {
    try {
        auto channel = ChannelFactory::create_client(io_, host_, port_, config_.codec);

        // 设置连接关闭回调
        channel->set_close_callback([this](const boost::system::error_code& ec) {
            if (ec) {
                RENDU_LOG_DEBUG("Connection closed for {}:{}: {}", host_, port_, ec.message());
            }
        });

        // 启动连接
        channel->start();

        return channel;
    } catch (const std::exception& e) {
        RENDU_LOG_ERROR("Failed to create connection for {}:{}: {}", host_, port_, e.what());
        return nullptr;
    }
}

bool ConnectionPool::is_connection_valid(const std::shared_ptr<Channel>& channel) const {
    if (!channel) {
        return false;
    }

    if (!channel->is_open()) {
        return false;
    }

    // TODO: 可以添加连接健康检查,如发送心跳等

    return true;
}

} // namespace net
END_NAMESPACE_COMMON
