#include "core/cluster/node_discovery.h"

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <random>
#include <sstream>
#include <iostream>

BEGIN_NAMESPACE_CORE

NodeDiscovery::NodeDiscovery(const Config& config)
    : config_(config) {

    if (config_.node_id.empty()) {
        config_.node_id = generate_node_id();
    }

    local_node_.node_id = config_.node_id;
    local_node_.address = config_.listen_address;
    local_node_.port = config_.listen_port;
    local_node_.first_seen = std::chrono::system_clock::now();
    local_node_.last_seen = local_node_.first_seen;
}

NodeDiscovery::~NodeDiscovery() {
    stop();
}

std::string NodeDiscovery::generate_node_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::ostringstream oss;
    oss << std::hex;

    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            oss << '-';
        }
        oss << dis(gen);
    }

    return oss.str();
}

bool NodeDiscovery::init_socket() {
    socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd_ < 0) {
        return false;
    }

    int broadcast_enable = 1;
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_BROADCAST,
                   &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    int reuse_addr = 1;
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR,
                   &reuse_addr, sizeof(reuse_addr)) < 0) {
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(config_.listen_port);

    if (bind(socket_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    // 如果绑定的是端口 0，获取系统分配的实际端口号
    if (config_.listen_port == 0) {
        socklen_t addr_len = sizeof(addr);
        if (getsockname(socket_fd_, (sockaddr*)&addr, &addr_len) == 0) {
            config_.listen_port = ntohs(addr.sin_port);
            local_node_.port = config_.listen_port;
        }
    }

    return true;
}

void NodeDiscovery::start() {
    if (running_.load()) {
        return;
    }

    if (!init_socket()) {
        throw std::runtime_error("Failed to initialize UDP socket for node discovery");
    }

    running_.store(true);

    listen_thread_ = std::make_unique<std::thread>(&NodeDiscovery::listen_loop, this);
    broadcast_thread_ = std::make_unique<std::thread>(&NodeDiscovery::broadcast_loop, this);
    liveness_thread_ = std::make_unique<std::thread>(&NodeDiscovery::check_liveness_loop, this);

    std::cout << "[NodeDiscovery] Started: " << local_node_.node_id << std::endl;
}

void NodeDiscovery::stop() {
    if (!running_.load()) {
        return;
    }

    running_.store(false);

    // 先关闭 socket 以唤醒 select
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }

    // 等待线程退出，最多等待1秒
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);

    if (listen_thread_ && listen_thread_->joinable()) {
        listen_thread_->join();
    }
    if (broadcast_thread_ && broadcast_thread_->joinable()) {
        broadcast_thread_->join();
    }
    if (liveness_thread_ && liveness_thread_->joinable()) {
        liveness_thread_->join();
    }

    listen_thread_.reset();
    broadcast_thread_.reset();
    liveness_thread_.reset();

    std::cout << "[NodeDiscovery] Stopped" << std::endl;
}

void NodeDiscovery::broadcast() {
    std::string announcement = build_announcement();

    sockaddr_in broadcast_addr;
    std::memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
    broadcast_addr.sin_port = htons(config_.broadcast_port);

    ssize_t sent = sendto(socket_fd_, announcement.c_str(), announcement.size(),
                          0, (sockaddr*)&broadcast_addr, sizeof(broadcast_addr));

    if (sent < 0) {
        std::cerr << "[NodeDiscovery] Failed to send broadcast" << std::endl;
    }
}

void NodeDiscovery::broadcast_loop() {
    while (running_.load()) {
        broadcast();
        // 分片睡眠以便 stop() 时能快速退出
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(config_.announce_interval);
        const auto chunk = std::chrono::milliseconds(100);
        while (remaining > std::chrono::milliseconds(0) && running_.load()) {
            std::this_thread::sleep_for(std::min(remaining, chunk));
            remaining -= chunk;
        }
    }
}

void NodeDiscovery::listen_loop() {
    char buffer[1024];
    sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    while (running_.load()) {
        fd_set read_set;
        FD_ZERO(&read_set);
        if (socket_fd_ >= 0) {
            FD_SET(socket_fd_, &read_set);

            timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 50000;  // 50ms

            int result = select(socket_fd_ + 1, &read_set, nullptr, nullptr, &timeout);

            if (result < 0) {
                break;  // 出错时退出
            }

            if (FD_ISSET(socket_fd_, &read_set)) {
                ssize_t received = recvfrom(socket_fd_, buffer, sizeof(buffer) - 1,
                                           0, (sockaddr*)&from_addr, &from_len);
                if (received > 0) {
                    buffer[received] = '\0';
                    char from_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, INET_ADDRSTRLEN);
                    handle_announcement(std::string(buffer, received), std::string(from_ip));
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void NodeDiscovery::check_liveness_loop() {
    while (running_.load()) {
        cleanup_expired_nodes();
        // 分片睡眠以便 stop() 时能快速退出
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(config_.cleanup_interval);
        const auto chunk = std::chrono::milliseconds(100);
        while (remaining > std::chrono::milliseconds(0) && running_.load()) {
            std::this_thread::sleep_for(std::min(remaining, chunk));
            remaining -= chunk;
        }
    }
}

void NodeDiscovery::handle_announcement(const std::string& data, const std::string& from_address) {
    auto node_info = parse_announcement(data);
    if (!node_info) {
        return;
    }

    if (node_info->node_id == local_node_.node_id) {
        return;
    }

    std::lock_guard<std::mutex> lock(nodes_mutex_);

    auto it = nodes_.find(node_info->node_id);
    if (it == nodes_.end()) {
        node_info->first_seen = std::chrono::system_clock::now();
        node_info->last_seen = node_info->first_seen;
        nodes_[node_info->node_id] = *node_info;

        NodeEvent event;
        event.type = NodeEvent::Type::Joined;
        event.node_info = *node_info;
        event.timestamp = std::chrono::system_clock::now();

        notify_node_change(event);

        std::cout << "[NodeDiscovery] Node joined: " << node_info->node_id << std::endl;
    } else {
        it->second.last_seen = std::chrono::system_clock::now();

        if (it->second.address != node_info->address || it->second.port != node_info->port) {
            it->second.address = node_info->address;
            it->second.port = node_info->port;

            NodeEvent event;
            event.type = NodeEvent::Type::Updated;
            event.node_info = it->second;
            event.timestamp = std::chrono::system_clock::now();

            notify_node_change(event);
        }
    }
}

std::string NodeDiscovery::build_announcement() {
    std::ostringstream oss;
    auto timestamp = std::chrono::system_clock::now().time_since_epoch();
    uint64_t ts = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp).count();

    oss << "NODE_DISCOVERY|"
        << "v1|"
        << local_node_.node_id << "|"
        << local_node_.address << "|"
        << local_node_.port << "|"
        << ts;

    return oss.str();
}

std::optional<NodeInfo> NodeDiscovery::parse_announcement(const std::string& data) {
    if (data.size() > 1024) {
        return std::nullopt;
    }

    std::istringstream iss(data);
    std::string protocol, version, node_id, address;
    uint16_t port;
    uint64_t timestamp;

    if (!(iss >> protocol >> version >> node_id >> address >> port >> timestamp)) {
        return std::nullopt;
    }

    if (protocol != "NODE_DISCOVERY" || version != "v1") {
        return std::nullopt;
    }

    NodeInfo info;
    info.node_id = node_id;
    info.address = address;
    info.port = port;
    info.last_seen = std::chrono::system_clock::now();

    return info;
}

void NodeDiscovery::notify_node_change(const NodeEvent& event) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    if (node_change_callback_) {
        node_change_callback_(event);
    }
}

std::vector<NodeInfo> NodeDiscovery::get_active_nodes() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    std::vector<NodeInfo> result;
    result.reserve(nodes_.size());

    auto now = std::chrono::system_clock::now();
    for (const auto& [id, node] : nodes_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - node.last_seen);
        if (elapsed <= config_.node_timeout) {
            result.push_back(node);
        }
    }

    return result;
}

size_t NodeDiscovery::get_node_count() const {
    return get_active_nodes().size();
}

const NodeInfo& NodeDiscovery::get_local_node() const {
    return local_node_;
}

std::optional<NodeInfo> NodeDiscovery::get_node(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.last_seen);
        if (elapsed <= config_.node_timeout) {
            return it->second;
        }
    }
    return std::nullopt;
}

void NodeDiscovery::on_node_change(NodeChangeCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    node_change_callback_ = std::move(callback);
}

void NodeDiscovery::remove_node_change_callback() {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    node_change_callback_ = nullptr;
}

void NodeDiscovery::add_node(const NodeInfo& node) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    auto it = nodes_.find(node.node_id);
    if (it == nodes_.end()) {
        NodeInfo new_node = node;
        new_node.first_seen = std::chrono::system_clock::now();
        new_node.last_seen = new_node.first_seen;
        nodes_[node.node_id] = new_node;

        NodeEvent event;
        event.type = NodeEvent::Type::Joined;
        event.node_info = new_node;
        event.timestamp = std::chrono::system_clock::now();
        notify_node_change(event);
    } else {
        // 检查是否需要更新节点信息
        if (it->second.address != node.address || it->second.port != node.port) {
            it->second.address = node.address;
            it->second.port = node.port;

            NodeEvent event;
            event.type = NodeEvent::Type::Updated;
            event.node_info = it->second;
            event.timestamp = std::chrono::system_clock::now();
            notify_node_change(event);
        }
        it->second.last_seen = std::chrono::system_clock::now();
    }
}

void NodeDiscovery::remove_node(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        NodeEvent event;
        event.type = NodeEvent::Type::Left;
        event.node_info = it->second;
        event.timestamp = std::chrono::system_clock::now();
        notify_node_change(event);

        nodes_.erase(it);
    }
}

void NodeDiscovery::cleanup_expired_nodes() {
    std::vector<std::string> expired_ids;

    {
        std::lock_guard<std::mutex> lock(nodes_mutex_);
        auto now = std::chrono::system_clock::now();

        for (auto it = nodes_.begin(); it != nodes_.end(); ) {
            if (it->second.is_expired(config_.node_timeout)) {
                expired_ids.push_back(it->first);

                NodeEvent event;
                event.type = NodeEvent::Type::Left;
                event.node_info = it->second;
                event.timestamp = now;
                notify_node_change(event);

                std::cout << "[NodeDiscovery] Node expired: " << it->second.node_id << std::endl;

                it = nodes_.erase(it);
            } else {
                ++it;
            }
        }
    }
}

END_NAMESPACE_CORE
