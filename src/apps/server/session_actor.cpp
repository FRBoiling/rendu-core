#include "session_actor.h"
#include "server_actor.h"
#include "server_messages.h"
#include "server_stats.h"
#include "common/log/logger.h"
#include <ctime>
#include <chrono>

using namespace Rendu;
using namespace server;

SessionActor::SessionActor(const std::string& name,
                           std::shared_ptr<Rendu::net::TcpSocket> socket,
                           int32_t user_id,
                           Rendu::ActorRef server_ref)
    : Actor(name), socket_(socket), codec_(std::make_unique<Rendu::net::LengthPrefixCodec>()),
      user_id_(user_id), server_ref_(server_ref), system_(nullptr) {
}

SessionActor::~SessionActor() {
    RENDU_LOG_DEBUG("[SessionActor] destroyed: user_id={}, username={}", user_id_, username_);
}

void SessionActor::receive(std::shared_ptr<Message> msg) {
    const std::string& type = msg->get_type();

    if (type == "SendToClientMessage") {
        auto* send_msg = dynamic_cast<server::SendToClientMessage*>(msg.get());
        if (send_msg) {
            send_raw_data(send_msg->data());
        }
    }
}

void SessionActor::on_start() {
    RENDU_LOG_INFO("[SessionActor] started: user_id={}", user_id_);
    // 开始接收数据
    start_receive();
}

void SessionActor::on_stop() {
    RENDU_LOG_INFO("[SessionActor] stopped: user_id={}, username={}, 已登录: {}",
                   user_id_, username_, logged_in_.load() ? "是" : "否");

    // 通知 Server Actor 会话结束
    if (system_) {
        auto logout_msg = std::make_shared<server::SessionLogoutMessage>(user_id_);
        system_->tell(server_ref_, logout_msg);
    }

    // 关闭 socket
    if (socket_) {
        socket_->close();
    }
}

void SessionActor::on_data_received(const std::vector<byte>& data) {
    try {
        std::lock_guard<std::mutex> lock(buffer_mutex_);

        // 将接收到的数据追加到缓冲区
        receive_buffer_.insert(receive_buffer_.end(), data.begin(), data.end());

        // 使用解码器提取完整消息
        auto messages = codec_->decode(receive_buffer_);

        RENDU_LOG_DEBUG("[SessionActor] 收到数据: user_id={}, 大小={}, 解析出 {} 条消息",
                       user_id_, data.size(), messages.size());

        // 更新统计
        if (stats_) {
            stats_->increment_bytes_received(data.size());
        }

        // 处理每个完整消息
        for (const auto& message_data : messages) {
            // 解析 protobuf 消息
            protocol::ClientMessage client_msg;
            if (!client_msg.ParseFromArray(message_data.data(),
                                          static_cast<int>(message_data.size()))) {
                RENDU_LOG_ERROR("[SessionActor] Failed to parse client message from user {}, 数据大小: {}",
                               user_id_, message_data.size());
                if (stats_) {
                    stats_->increment_parse_errors();
                }
                continue;
            }

            // 更新统计
            if (stats_) {
                stats_->increment_messages_received();
            }

            // 根据消息类型处理
            if (client_msg.has_login()) {
                on_login(client_msg.login());
            } else if (client_msg.has_chat()) {
                on_chat(client_msg.chat());
            } else {
                RENDU_LOG_WARN("[SessionActor] Unknown message type from user {}", user_id_);
            }
        }
    } catch (const std::exception& e) {
        RENDU_LOG_ERROR("[SessionActor] Error processing data from user {}: {}", user_id_, e.what());
    }
}

void SessionActor::on_login(const protocol::LoginRequest& request) {
    RENDU_LOG_INFO("[SessionActor] Login request: user_id={}, username={}", user_id_, request.username());

    // 简单验证（实际应用中应该验证密码）
    bool success = true;
    if (success) {
        logged_in_ = true;
        username_ = request.username();

        // 通知 Server Actor 登录成功
        if (system_) {
            auto login_msg = std::make_shared<server::SessionLoginMessage>(user_id_, username_);
            system_->tell(server_ref_, login_msg);
        }
    }

    // 发送登录响应
    protocol::ServerMessage server_msg;
    auto* response = server_msg.mutable_login_response();
    response->set_success(success);
    response->set_user_id(user_id_);
    response->set_message(success ? "Login successful" : "Login failed");

    send_to_client(server_msg);
}

void SessionActor::on_chat(const protocol::ChatMessage& message) {
    if (!logged_in_) {
        RENDU_LOG_WARN("[SessionActor] Chat message from unauthenticated user {}", user_id_);
        return;
    }

    auto start_time = std::chrono::steady_clock::now();

    RENDU_LOG_DEBUG("[SessionActor] Chat from {}: {}", username_, message.content());

    // 设置时间戳和用户信息
    auto* mutable_msg = const_cast<protocol::ChatMessage*>(&message);
    mutable_msg->set_from_user_id(user_id_);
    mutable_msg->set_from_username(username_);
    mutable_msg->set_timestamp(std::time(nullptr));

    // 通知 Server Actor 广播消息
    if (system_) {
        auto broadcast_msg = std::make_shared<server::BroadcastChatMessage>(
            user_id_, username_, message.content()
        );
        system_->tell(server_ref_, broadcast_msg);
    }

    // 更新延迟统计
    auto end_time = std::chrono::steady_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    if (stats_) {
        stats_->update_avg_message_latency(latency);
    }
}

void SessionActor::send_to_client(const protocol::ServerMessage& msg) {
    if (!socket_) {
        RENDU_LOG_WARN("[SessionActor] Attempt to send but socket is null: user_id={}", user_id_);
        return;
    }

    // 序列化 protobuf 消息
    std::string serialized;
    if (!msg.SerializeToString(&serialized)) {
        RENDU_LOG_ERROR("[SessionActor] Failed to serialize server message for user {}", user_id_);
        return;
    }

    // 使用编码器添加长度前缀
    std::vector<byte> data(serialized.begin(), serialized.end());
    auto encoded_data = codec_->encode(data);

    RENDU_LOG_DEBUG("[SessionActor] Sending to client: user_id={}, 大小={}",
                   user_id_, encoded_data.size());

    // 发送编码后的数据
    socket_->async_send(encoded_data, [this, user_id = user_id_]
                        (const boost::system::error_code& ec, size_t bytes_sent) {
        if (ec) {
            RENDU_LOG_ERROR("[SessionActor] Send error for user {}: {} (发送字节: {})",
                           user_id, ec.message(), bytes_sent);
            // 更新统计
            if (stats_) {
                stats_->increment_send_errors();
            }
            // 停止 Actor
            if (system_) {
                system_->stop_actor(self());
            }
        } else {
            RENDU_LOG_DEBUG("[SessionActor] Send success for user {}: {} bytes", user_id, bytes_sent);
        }
    });
}

void SessionActor::send_raw_data(const std::vector<byte>& data) {
    if (!socket_) {
        RENDU_LOG_WARN("[SessionActor] Attempt to send raw data but socket is null: user_id={}", user_id_);
        return;
    }

    RENDU_LOG_DEBUG("[SessionActor] Sending raw data: user_id={}, 大小={}", user_id_, data.size());

    // 直接发送原始数据（假设已经编码）
    socket_->async_send(data, [this, user_id = user_id_]
                        (const boost::system::error_code& ec, size_t bytes_sent) {
        if (ec) {
            RENDU_LOG_ERROR("[SessionActor] Send error for user {}: {} (发送字节: {})",
                           user_id, ec.message(), bytes_sent);
            // 更新统计
            if (stats_) {
                stats_->increment_send_errors();
            }
            if (system_) {
                system_->stop_actor(self());
            }
        } else {
            RENDU_LOG_DEBUG("[SessionActor] Raw data send success for user {}: {} bytes",
                           user_id, bytes_sent);
        }
    });
}

void SessionActor::start_receive() {
    if (!socket_) {
        RENDU_LOG_WARN("[SessionActor] Attempt to start receive but socket is null: user_id={}", user_id_);
        return;
    }

    socket_->async_receive(4096, [this](const boost::system::error_code& ec, std::vector<byte> data) {
        if (ec) {
            if (ec != boost::asio::error::operation_aborted) {
                RENDU_LOG_ERROR("[SessionActor] Receive error for user {}: {}",
                               user_id_, ec.message());
            }
            // 停止 Actor
            if (system_) {
                system_->stop_actor(self());
            }
            return;
        }

        // 处理接收到的数据
        on_data_received(data);

        // 继续接收
        if (is_started() && !is_stopped()) {
            start_receive();
        }
    });
}

