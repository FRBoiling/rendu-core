#include "core/actor/remote_actor_system.h"
#include "core/actor/message.h"
#include "core/actor/actor.h"
#include "common/net/remote_channel.h"
#include "common/log/logger.h"
#include "common/io/io_context.h"
#include "common/ser/protobuf_ser.h"
#include "remote_message.pb.h"
#include <sstream>
#include <thread>
#include <chrono>

BEGIN_NAMESPACE_CORE

RemoteActorSystem::RemoteActorSystem(const Config& config)
    : ActorSystem(config.actor_thread_pool_size)
    , config_(config)
    , node_id_(config.node_id)
    , io_context_(std::make_shared<COMMON_NAMESPACE::io::IoContext>(config.io_threads)) {
    RENDU_LOG_INFO("RemoteActorSystem created with node_id: {} (actor_threads: {}, io_threads: {})",
              node_id_, config.actor_thread_pool_size, config.io_threads);
}

RemoteActorSystem::~RemoteActorSystem() {
    stop();
}

void RemoteActorSystem::start() {
    // 先启动基类
    ActorSystem::start();

    // 启动 IoContext（需要单独的线程运行）
    io_thread_ = std::thread([this]() {
        RENDU_LOG_INFO("IoContext thread started");
        io_context_->run();
        RENDU_LOG_INFO("IoContext thread stopped");
    });

    // 初始化远程连接
    init_remote_connections();

    RENDU_LOG_INFO("RemoteActorSystem started (node_id: {})", node_id_);
}

void RemoteActorSystem::stop() {
    RENDU_LOG_INFO("Stopping RemoteActorSystem (node_id: {})...", node_id_);

    // 断开所有远程连接
    {
        std::lock_guard<std::mutex> lock(channels_mutex_);
        for (auto& [addr, channel] : node_channels_) {
            channel->disconnect();
        }
        node_channels_.clear();
    }

    // 停止 IoContext
    if (io_context_) {
        io_context_->stop();
    }

    // 等待 IoContext 线程结束
    if (io_thread_.joinable()) {
        io_thread_.join();
    }

    // 停止基类
    ActorSystem::stop();

    RENDU_LOG_INFO("RemoteActorSystem stopped");
}

void RemoteActorSystem::init_remote_connections() {
    RENDU_LOG_INFO("Initializing remote connections...");

    for (const auto& node_addr : config_.known_nodes) {
        connect_to_node(node_addr);
    }

    RENDU_LOG_INFO("Remote connections initialized: {}", config_.known_nodes.size());
}

void RemoteActorSystem::connect_to_node(const std::string& node_addr) {
    std::string address;
    uint16_t port = 0;

    if (!parse_node_address(node_addr, address, port)) {
        RENDU_LOG_ERROR("Invalid node address: {}", node_addr);
        return;
    }

    // 检查是否已经存在连接
    {
        std::lock_guard<std::mutex> lock(channels_mutex_);
        if (node_channels_.find(node_addr) != node_channels_.end()) {
            RENDU_LOG_WARN("Connection to {} already exists", node_addr);
            return;
        }
    }

    // 创建 RemoteChannel
    COMMON_NAMESPACE::net::RemoteChannel::Config channel_config;
    channel_config.node_id = node_id_;
    channel_config.auto_reconnect = true;
    channel_config.reconnect_interval = 5000;  // 5 秒
    channel_config.max_reconnect_attempts = 0;  // 无限重连

    auto channel = std::make_shared<COMMON_NAMESPACE::net::RemoteChannel>(channel_config, io_context_);

    // 设置回调
    channel->set_connect_callback([this, node_addr]() {
        RENDU_LOG_INFO("Connected to remote node: {}", node_addr);
    });

    channel->set_disconnect_callback([this, node_addr]() {
        RENDU_LOG_WARN("Disconnected from remote node: {}", node_addr);
    });

    channel->set_message_callback([this](const std::shared_ptr<rendu::remote::RemoteMessage>& msg) {
        handle_remote_message(msg);
    });

    channel->set_request_callback([this](const std::shared_ptr<rendu::remote::RemoteRequest>& req) {
        handle_remote_request(req);
    });

    channel->set_response_callback([this](const std::shared_ptr<rendu::remote::RemoteResponse>& resp) {
        handle_remote_response(resp);
    });

    channel->set_error_callback([this, node_addr](const std::string& error) {
        RENDU_LOG_ERROR("Remote channel error for {}: {}", node_addr, error);
    });

    // 连接到远程节点
    channel->connect(address, port);

    // 添加到连接池
    {
        std::lock_guard<std::mutex> lock(channels_mutex_);
        node_channels_[node_addr] = channel;
    }

    RENDU_LOG_INFO("Connecting to remote node: {} ({}:{})", node_addr, address, port);
}

bool RemoteActorSystem::parse_node_address(const std::string& node_addr,
                                          std::string& output_address,
                                          uint16_t& output_port) const {
    size_t pos = node_addr.rfind(':');
    if (pos == std::string::npos) {
        return false;
    }

    output_address = node_addr.substr(0, pos);
    std::string port_str = node_addr.substr(pos + 1);

    try {
        output_port = static_cast<uint16_t>(std::stoul(port_str));
        return output_port > 0;
    } catch (const std::exception& e) {
        RENDU_LOG_ERROR("Failed to parse port from {}: {}", node_addr, e.what());
        return false;
    }
}

void RemoteActorSystem::tell(const ActorRef& target, std::shared_ptr<Message> msg) {
    if (is_local_actor(target)) {
        // 本地 Actor：使用基类的 tell
        ActorSystem::tell(target, msg);
        return;
    }

    // 远程 Actor：发送远程消息
    std::string node_addr = extract_node_address(target);
    if (node_addr.empty()) {
        RENDU_LOG_ERROR("Failed to extract node address from ActorRef: {}", target.to_string());
        return;
    }

    // 创建远程消息
    rendu::remote::RemoteMessage remote_message;
    auto* sender = remote_message.mutable_sender();
    sender->set_path("");  // TODO: 发送者路径（需要当前 ActorRef）
    sender->set_actor_id(0);

    auto* receiver = remote_message.mutable_receiver();
    receiver->set_path(target.path());
    receiver->set_actor_id(target.actor_id());

    auto* body = remote_message.mutable_body();
    body->set_type(msg->type());

    // 序列化消息（使用 Message 自带的序列化）
    std::string serialized_msg = msg->serialize();
    body->set_payload(serialized_msg);

    send_remote_message(node_addr, remote_message);

    RENDU_LOG_DEBUG("Sending remote message to {}:{} (node: {})", target.path(), target.actor_id(), node_addr);
}

std::shared_ptr<Message> RemoteActorSystem::ask(const ActorRef& target,
                                                std::shared_ptr<Message> msg,
                                                uint64_t timeout_ms) {
    if (is_local_actor(target)) {
        // 本地 Actor：使用基类的 ask
        return ActorSystem::ask(target, msg, timeout_ms);
    }

    // 远程 Actor：发送远程请求
    std::string node_addr = extract_node_address(target);
    if (node_addr.empty()) {
        RENDU_LOG_ERROR("Failed to extract node address from ActorRef: {}", target.to_string());
        return nullptr;
    }

    // 创建远程请求
    uint64_t request_id = generate_request_id();
    rendu::remote::RemoteRequest remote_request;

    auto* sender = remote_request.mutable_sender();
    sender->set_path("");  // TODO: 发送者路径
    sender->set_actor_id(0);

    auto* receiver = remote_request.mutable_receiver();
    receiver->set_path(target.path());
    receiver->set_actor_id(target.actor_id());

    auto* body = remote_request.mutable_body();
    body->set_type(msg->type());

    // 序列化消息（使用 Message 自带的序列化）
    std::string serialized_msg = msg->serialize();
    body->set_payload(serialized_msg);

    remote_request.set_request_id(request_id);

    // 创建 Promise 等待响应
    auto promise = std::make_shared<MessagePromise>();
    {
        std::lock_guard<std::mutex> lock(requests_mutex_);
        pending_requests_[request_id] = promise;
    }

    send_remote_request(node_addr, remote_request, request_id);

    // 等待响应
    auto future = promise->get_future();
    if (timeout_ms == 0) {
        return future.get();
    } else {
        // TODO: 需要实现 wait_for
        // return promise->wait_for(timeout_ms);
        return future.get();  // 暂时阻塞等待
    }
}

bool RemoteActorSystem::is_local_actor(const ActorRef& ref) const {
    // 根据路径判断是否为本地 Actor
    // 这里假设本地 Actor 的路径不包含节点信息
    // 远程 Actor 的路径可能包含 /node:port/ 前缀
    return !ref.path().empty() && ref.path().find('/') == 0;
}

std::string RemoteActorSystem::extract_node_address(const ActorRef& ref) const {
    // 从 ActorRef 中提取节点地址
    // 假设远程 Actor 的路径格式为: /node:addr:port/path
    const std::string& path = ref.path();

    size_t first_slash = path.find('/');
    size_t second_slash = path.find('/', first_slash + 1);

    if (first_slash == 0 && second_slash != std::string::npos) {
        std::string node_info = path.substr(first_slash + 1, second_slash - first_slash - 1);
        // 检查是否为节点信息（格式：node:addr:port）
        if (node_info.substr(0, 5) == "node:") {
            return node_info.substr(5);  // 返回 addr:port
        }
    }

    return "";  // 本地 Actor
}

void RemoteActorSystem::send_remote_message(const std::string& node_addr,
                                            const rendu::remote::RemoteMessage& remote_message) {
    std::lock_guard<std::mutex> lock(channels_mutex_);

    auto it = node_channels_.find(node_addr);
    if (it == node_channels_.end() || !it->second) {
        RENDU_LOG_ERROR("No channel to node: {}", node_addr);
        return;
    }

    it->second->send_message(remote_message);
}

void RemoteActorSystem::send_remote_request(const std::string& node_addr,
                                            const rendu::remote::RemoteRequest& remote_request,
                                            uint64_t request_id) {
    std::lock_guard<std::mutex> lock(channels_mutex_);

    auto it = node_channels_.find(node_addr);
    if (it == node_channels_.end() || !it->second) {
        RENDU_LOG_ERROR("No channel to node: {}", node_addr);
        return;
    }

    it->second->send_request(remote_request);
}

void RemoteActorSystem::send_remote_response(const std::string& node_addr,
                                               const rendu::remote::RemoteResponse& remote_response) {
    std::lock_guard<std::mutex> lock(channels_mutex_);

    auto it = node_channels_.find(node_addr);
    if (it == node_channels_.end() || !it->second) {
        RENDU_LOG_ERROR("No channel to node: {}", node_addr);
        return;
    }

    it->second->send_response(remote_response);
}

void RemoteActorSystem::handle_remote_message(const std::shared_ptr<rendu::remote::RemoteMessage>& remote_message) {
    // 反序列化消息
    const auto& body = remote_message->body();

    // Tell 模式不需要 request_id，直接使用 payload
    std::string serialized_data = body.payload();
    auto msg = MessageRegistry::deserialize(body.type() + "|" + serialized_data);

    if (!msg) {
        RENDU_LOG_ERROR("Failed to deserialize remote message of type: {}", body.type());
        return;
    }

    // Tell 模式不设置 request_id（单向消息）
    // msg->set_request_id(0);

    // 构造本地 ActorRef
    ActorRef receiver(body.type(), remote_message->receiver().actor_id());

    // 发送给本地 Actor
    tell(receiver, msg);

    RENDU_LOG_DEBUG("Received remote message for {}", receiver.to_string());
}

void RemoteActorSystem::handle_remote_request(const std::shared_ptr<rendu::remote::RemoteRequest>& remote_request) {
    uint64_t request_id = remote_request->request_id();
    const auto& body = remote_request->body();

    RENDU_LOG_DEBUG("Received remote request {} for {}", request_id, remote_request->receiver().path());

    // 使用 MessageRegistry 反序列化请求消息
    std::string serialized_data = std::to_string(remote_request->request_id()) + "|" + body.payload();
    auto msg = MessageRegistry::deserialize(body.type() + "|" + serialized_data);

    if (!msg) {
        RENDU_LOG_ERROR("Failed to deserialize remote request of type: {}", body.type());
        return;
    }

    msg->set_request_id(request_id);

    // 构造本地 ActorRef
    ActorRef receiver(body.type(), remote_request->receiver().actor_id());

    // 临时请求 ID（用于响应）
    uint64_t temp_request_id = generate_request_id();
    auto promise = std::make_shared<MessagePromise>();
    {
        std::lock_guard<std::mutex> lock(requests_mutex_);
        pending_requests_[temp_request_id] = promise;
    }

    // 使用 ask 获取本地 Actor 的响应
    auto response = ask(receiver, msg);

    // 构造远程响应
    rendu::remote::RemoteResponse remote_response;
    auto* sender = remote_response.mutable_sender();
    sender->set_path(receiver.path());
    sender->set_actor_id(receiver.actor_id());

    auto* resp_receiver = remote_response.mutable_receiver();
    resp_receiver->set_path(remote_request->sender().path());
    resp_receiver->set_actor_id(remote_request->sender().actor_id());

    auto* resp_body = remote_response.mutable_body();
    resp_body->set_type(response ? response->type() : "");

    // 序列化响应
    if (response) {
        std::string serialized_resp = response->serialize();
        resp_body->set_payload(serialized_resp);
    }

    remote_response.set_request_id(request_id);

    // 发送响应
    std::string node_addr = extract_node_address(ActorRef(remote_request->sender().path(),
                                                           remote_request->sender().actor_id()));
    send_remote_response(node_addr, remote_response);
}

void RemoteActorSystem::handle_remote_response(const std::shared_ptr<rendu::remote::RemoteResponse>& remote_response) {
    uint64_t request_id = remote_response->request_id();

    RENDU_LOG_DEBUG("Received remote response {}", request_id);

    // 查找对应的 Promise
    std::shared_ptr<MessagePromise> promise;
    {
        std::lock_guard<std::mutex> lock(requests_mutex_);
        auto it = pending_requests_.find(request_id);
        if (it == pending_requests_.end()) {
            RENDU_LOG_WARN("No pending request for id: {}", request_id);
            return;
        }
        promise = it->second;
        pending_requests_.erase(it);
    }

    // 反序列化响应
    const auto& body = remote_response->body();
    std::shared_ptr<Message> response = nullptr;

    if (!body.type().empty() && !body.payload().empty()) {
        // 使用 MessageRegistry 反序列化响应
        std::string serialized_data = std::to_string(remote_response->request_id()) + "|" + body.payload();
        response = MessageRegistry::deserialize(body.type() + "|" + serialized_data);

        if (!response) {
            RENDU_LOG_ERROR("Failed to deserialize remote response of type: {}", body.type());
        }
    }

    // 设置响应
    promise->set_value(response);
}

rendu::remote::RemoteActorRef RemoteActorSystem::to_remote_actor_ref(const ActorRef& ref) const {
    rendu::remote::RemoteActorRef remote_ref;
    remote_ref.set_path(ref.path());
    remote_ref.set_actor_id(ref.actor_id());
    return remote_ref;
}

uint64_t RemoteActorSystem::generate_request_id() {
    return next_request_id_.fetch_add(1);
}

END_NAMESPACE_CORE
