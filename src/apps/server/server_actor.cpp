#include "server_actor.h"
#include "session_actor.h"
#include "server_messages.h"
#include "server_stats.h"
#include "common/log/logger.h"
#include <ctime>

using namespace Rendu;
using namespace server;

ServerActor::ServerActor(const std::string& name, Rendu::ActorSystem* system)
    : Actor(name), system_(system) {
}

ServerActor::~ServerActor() {
    RENDU_LOG_DEBUG("[ServerActor] destroyed: {}", name());
}

void ServerActor::receive(std::shared_ptr<Message> msg) {
    const std::string& type = msg->get_type();

    if (type == "SessionLoginMessage") {
        auto* login_msg = dynamic_cast<server::SessionLoginMessage*>(msg.get());
        if (login_msg) {
            on_session_login(login_msg->user_id(), login_msg->username(), self());
        }
    } else if (type == "SessionLogoutMessage") {
        auto* logout_msg = dynamic_cast<server::SessionLogoutMessage*>(msg.get());
        if (logout_msg) {
            on_session_logout(logout_msg->user_id());
        }
    } else if (type == "BroadcastChatMessage") {
        auto* broadcast_msg = dynamic_cast<server::BroadcastChatMessage*>(msg.get());
        if (broadcast_msg) {
            on_broadcast_chat(broadcast_msg->from_user_id(),
                            broadcast_msg->from_username(),
                            broadcast_msg->content());
        }
    } else {
        RENDU_LOG_WARN("[ServerActor] Unknown message type: {}", type);
    }
}

void ServerActor::on_start() {
    RENDU_LOG_INFO("[ServerActor] started: {}", name());
}

void ServerActor::on_stop() {
    RENDU_LOG_INFO("[ServerActor] stopped: {}", name());

    // 停止所有会话
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    size_t session_count = sessions_.size();
    for (auto& [user_id, session_ref] : sessions_) {
        if (system_) {
            system_->stop_actor(session_ref);
        }
    }
    sessions_.clear();
    usernames_.clear();
    RENDU_LOG_INFO("[ServerActor] 已停止 {} 个会话", session_count);

    // 更新统计
    if (stats_) {
        std::string report = stats_->get_report();
        log::default_logger().info(report);
    }
}

void ServerActor::add_session(int32_t user_id, Rendu::ActorRef session_ref) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_[user_id] = session_ref;
    RENDU_LOG_DEBUG("[ServerActor] Session added: user_id={}, 总会话数: {}",
                   user_id, sessions_.size());
}

void ServerActor::remove_session(int32_t user_id) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_.erase(user_id);
    usernames_.erase(user_id);
    RENDU_LOG_DEBUG("[ServerActor] Session removed: user_id={}, 剩余会话数: {}",
                   user_id, sessions_.size());
}

void ServerActor::broadcast(const protocol::ChatMessage& msg, int32_t exclude_user_id) {
    protocol::ServerMessage server_msg;
    auto* chat = server_msg.mutable_chat();
    *chat = msg;

    // 序列化消息
    std::string serialized;
    if (!server_msg.SerializeToString(&serialized)) {
        RENDU_LOG_ERROR("[ServerActor] Failed to serialize broadcast message");
        if (stats_) {
            stats_->increment_send_errors();
        }
        return;
    }

    std::vector<byte> data(serialized.begin(), serialized.end());

    // 广播给所有会话（除发送者外）
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    int broadcast_count = 0;
    for (auto& [user_id, session_ref] : sessions_) {
        if (user_id != exclude_user_id && system_) {
            // 创建发送消息并通过 Actor 系统传递
            auto send_msg = std::make_shared<server::SendToClientMessage>(data);
            system_->tell(session_ref, send_msg);
            broadcast_count++;
        }
    }

    if (broadcast_count > 0) {
        RENDU_LOG_DEBUG("[ServerActor] 消息广播给 {} 个用户, 排除 user_id: {}",
                       broadcast_count, exclude_user_id);

        // 更新统计
        if (stats_) {
            stats_->increment_messages_sent();
            stats_->increment_bytes_sent(data.size());
        }
    }
}

int32_t ServerActor::next_user_id() {
    return ++next_user_id_;
}

void ServerActor::on_session_login(int32_t user_id, const std::string& username,
                                   const Rendu::ActorRef& session_ref) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_[user_id] = session_ref;
    usernames_[user_id] = username;

    RENDU_LOG_INFO("[ServerActor] User logged in: user_id={}, username={}, 总会话数: {}",
                   user_id, username, sessions_.size());
}

void ServerActor::on_session_logout(int32_t user_id) {
    std::string username;
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        auto it = usernames_.find(user_id);
        if (it != usernames_.end()) {
            username = it->second;
        }
    }

    RENDU_LOG_INFO("[ServerActor] User logged out: user_id={}, username={}", user_id, username);

    // 更新统计
    if (stats_) {
        stats_->decrement_active_connections();
    }

    remove_session(user_id);
}

void ServerActor::on_broadcast_chat(int32_t from_user_id, const std::string& from_username,
                                    const std::string& content) {
    protocol::ChatMessage chat_msg;
    chat_msg.set_from_user_id(from_user_id);
    chat_msg.set_from_username(from_username);
    chat_msg.set_content(content);
    chat_msg.set_timestamp(std::time(nullptr));

    RENDU_LOG_DEBUG("[ServerActor] 处理广播消息: from_user={}, content=\"{}\"",
                   from_username, content);

    broadcast(chat_msg, from_user_id);
}

