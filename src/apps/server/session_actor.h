#pragma once

#include "core/actor/actor.h"
#include "core/actor/actor_ref.h"
#include "core/actor/message.h"
#include "common/net/socket.h"
#include "common/net/codec.h"
#include "messages.pb.h"
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>

BEGIN_NAMESPACE_CORE
class ActorSystem;
END_NAMESPACE_CORE

namespace server {

// 前向声明
class ServerStats;

/// Session Actor - 管理单个客户端连接
class SessionActor : public Rendu::Actor {
public:
    /// 构造函数
    /// @param name Actor 名称
    /// @param socket TCP Socket
    /// @param user_id 用户 ID
    /// @param server_ref Server Actor 引用
    SessionActor(const std::string& name,
                 std::shared_ptr<Rendu::net::TcpSocket> socket,
                 int32_t user_id,
                 Rendu::ActorRef server_ref);

    /// 析构函数
    ~SessionActor() override;

    /// 处理消息
    void receive(std::shared_ptr<Rendu::Message> msg) override;

    /// 停止时的回调
    void on_stop() override;

    /// 设置 Actor 系统
    void set_system(Rendu::ActorSystem* system) { system_ = system; }

    /// 设置统计对象
    void set_stats(std::shared_ptr<ServerStats> stats) { stats_ = stats; }

    /// 启动时的回调
    void on_start() override;

private:
    /// 处理接收到的数据
    void on_data_received(const std::vector<byte>& data);

    /// 处理登录请求
    void on_login(const protocol::LoginRequest& request);

    /// 处理聊天消息
    void on_chat(const protocol::ChatMessage& message);

    /// 发送消息到客户端
    void send_to_client(const protocol::ServerMessage& msg);

    /// 发送原始数据到客户端
    void send_raw_data(const std::vector<byte>& data);

    /// 开始异步接收数据
    void start_receive();

private:
    std::shared_ptr<Rendu::net::TcpSocket> socket_;
    std::unique_ptr<Rendu::net::LengthPrefixCodec> codec_;
    int32_t user_id_;
    std::string username_;
    Rendu::ActorRef server_ref_;
    Rendu::ActorSystem* system_;
    std::atomic<bool> logged_in_{false};
    std::vector<byte> receive_buffer_;
    std::mutex buffer_mutex_;
    std::shared_ptr<ServerStats> stats_;
};

} // namespace server
