#pragma once

#include "core/define.h"
#include "core/actor/actor.h"
#include "core/actor/actor_ref.h"
#include "core/actor/message.h"
#include "common/net/remote_channel.h"
#include <memory>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <thread>

// 前向声明
namespace rendu {
namespace remote {
    class RemoteMessage;
    class RemoteRequest;
    class RemoteResponse;
    class RemoteActorRef;
}
}

BEGIN_NAMESPACE_CORE

// MessagePromise 已在 message.h 中定义，无需重复定义

/**
 * @brief 远程 Actor 系统
 *
 * 扩展 ActorSystem，支持跨节点 Actor 通信
 * - 节点 ID 管理
 * - RemoteChannel 连接池
 * - 本地/远程消息路由
 * - Ask 模式请求-响应匹配
 */
class RemoteActorSystem : public ActorSystem {
public:
    /**
     * @brief 配置
     */
    struct Config {
        std::string node_id;                   // 节点 ID
        std::string listen_address;             // 监听地址
        uint16_t listen_port;                  // 监听端口
        std::vector<std::string> known_nodes;  // 已知节点列表（格式: "host:port"）
        size_t actor_thread_pool_size{4};      // Actor 线程池大小
        size_t io_threads{2};                   // IO 线程数量（用于网络 I/O）
    };

    /**
     * @brief 构造函数
     */
    explicit RemoteActorSystem(const Config& config);

    /**
     * @brief 析构函数
     */
    ~RemoteActorSystem();

    // 禁止拷贝
    RemoteActorSystem(const RemoteActorSystem&) = delete;
    RemoteActorSystem& operator=(const RemoteActorSystem&) = delete;

    /**
     * @brief 启动远程 Actor 系统
     */
    void start();

    /**
     * @brief 覆盖 tell：支持跨节点消息
     * @param target 目标 ActorRef（可能为远程）
     * @param msg 消息
     */
    void tell(const ActorRef& target, std::shared_ptr<Message> msg);

    /**
     * @brief 覆盖 ask：支持跨节点请求
     * @param target 目标 ActorRef（可能为远程）
     * @param msg 请求消息
     * @param timeout_ms 超时时间（毫秒）
     * @return 响应消息或 nullptr（超时）
     */
    std::shared_ptr<Message> ask(const ActorRef& target,
                                  std::shared_ptr<Message> msg,
                                  uint64_t timeout_ms = 0);

    /**
     * @brief 停止远程 Actor 系统
     */
    void stop();

    /**
     * @brief 获取节点 ID
     */
    const std::string& node_id() const { return node_id_; }

private:
    /**
     * @brief 初始化远程节点连接
     */
    void init_remote_connections();

    /**
     * @brief 连接到指定节点
     * @param node_addr 节点地址（格式: "host:port"）
     */
    void connect_to_node(const std::string& node_addr);

    /**
     * @brief 判断 Actor 是否在本地
     * @param ref Actor 引用
     * @return 是否为本地 Actor
     */
    bool is_local_actor(const ActorRef& ref) const;

    /**
     * @brief 解析节点地址
     * @param node_addr 节点地址（格式: "host:port"）
     * @param output_address 输出主机地址
     * @param output_port 输出端口号
     * @return 是否解析成功
     */
    bool parse_node_address(const std::string& node_addr,
                          std::string& output_address, 
                          uint16_t& output_port) const;

    /**
     * @brief 发送远程消息（Tell 模式）
     * @param node_addr 节点地址
     * @param remote_message 远程消息
     */
    void send_remote_message(const std::string& node_addr,
                          const rendu::remote::RemoteMessage& remote_message);

    /**
     * @brief 发送远程请求（Ask 模式）
     * @param node_addr 节点地址
     * @param remote_request 远程请求
     * @param request_id 请求 ID
     */
    void send_remote_request(const std::string& node_addr,
                          const rendu::remote::RemoteRequest& remote_request,
                          uint64_t request_id);

    /**
     * @brief 发送远程响应（Ask 模式）
     * @param node_addr 节点地址
     * @param remote_response 远程响应
     */
    void send_remote_response(const std::string& node_addr,
                           const rendu::remote::RemoteResponse& remote_response);

    /**
     * @brief 处理接收到的远程消息
     * @param remote_message 远程消息
     */
    void handle_remote_message(const std::shared_ptr<rendu::remote::RemoteMessage>& remote_message);

    /**
     * @brief 处理接收到的远程请求
     * @param remote_request 远程请求
     */
    void handle_remote_request(const std::shared_ptr<rendu::remote::RemoteRequest>& remote_request);

    /**
     * @brief 处理接收到的远程响应
     * @param remote_response 远程响应
     */
    void handle_remote_response(const std::shared_ptr<rendu::remote::RemoteResponse>& remote_response);

    /**
     * @brief 转换 ActorRef 为 RemoteActorRef
     */
    rendu::remote::RemoteActorRef to_remote_actor_ref(const ActorRef& ref) const;

    /**
     * @brief 从 ActorRef 提取节点地址
     * @param ref Actor 引用
     * @return 节点地址（格式: "host:port"）或空字符串（本地）
     */
    std::string extract_node_address(const ActorRef& ref) const;

    /**
     * @brief 生成唯一的请求 ID
     */
    uint64_t generate_request_id();

private:
    // 配置
    Config config_;
    std::string node_id_;

    // IoContext（用于网络 I/O）
    std::shared_ptr<COMMON_NAMESPACE::io::IoContext> io_context_;
    std::thread io_thread_;

    // 远程节点连接池
    std::unordered_map<std::string, std::shared_ptr<COMMON_NAMESPACE::net::RemoteChannel>> node_channels_;
    mutable std::mutex channels_mutex_;

    // Ask 模式的请求管理
    std::atomic<uint64_t> next_request_id_{1};
    std::unordered_map<uint64_t, std::shared_ptr<MessagePromise>> pending_requests_;
    mutable std::mutex requests_mutex_;
};

END_NAMESPACE_CORE
