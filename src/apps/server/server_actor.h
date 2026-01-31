#pragma once

#include "core/actor/actor.h"
#include "core/actor/actor_ref.h"
#include "core/actor/message.h"
#include "messages.pb.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <mutex>

BEGIN_NAMESPACE_CORE
class ActorSystem;
END_NAMESPACE_CORE

namespace server {

// 前向声明
class SessionActor;
class ServerStats;

/// Server Actor - 管理所有会话和全局消息
class ServerActor : public Rendu::Actor {
public:
    /// 构造函数
    /// @param name Actor 名称
    /// @param system Actor 系统指针
    explicit ServerActor(const std::string& name, Rendu::ActorSystem* system = nullptr);

    /// 析构函数
    ~ServerActor() override;

    /// 处理消息
    void receive(std::shared_ptr<Rendu::Message> msg) override;

    /// 启动时的回调
    void on_start() override;

    /// 停止时的回调
    void on_stop() override;

    /// 设置统计对象
    void set_stats(std::shared_ptr<ServerStats> stats) { stats_ = stats; }

    /// 添加会话
    void add_session(int32_t user_id, Rendu::ActorRef session_ref);

    /// 移除会话
    void remove_session(int32_t user_id);

    /// 广播消息（除发送者外）
    void broadcast(const protocol::ChatMessage& msg, int32_t exclude_user_id = -1);

    /// 获取下一个用户 ID
    int32_t next_user_id();

private:
    /// 处理新用户 ID 请求
    void on_new_user_id(const Rendu::ActorRef& sender);

    /// 处理会话登录
    void on_session_login(int32_t user_id, const std::string& username, const Rendu::ActorRef& session_ref);

    /// 处理会话登出
    void on_session_logout(int32_t user_id);

    /// 处理广播聊天
    void on_broadcast_chat(int32_t from_user_id, const std::string& from_username,
                           const std::string& content);

private:
    Rendu::ActorSystem* system_;
    std::unordered_map<int32_t, Rendu::ActorRef> sessions_;
    std::unordered_map<int32_t, std::string> usernames_;
    std::mutex sessions_mutex_;
    std::atomic<int32_t> next_user_id_{0};
    std::shared_ptr<ServerStats> stats_;
};

} // namespace server
