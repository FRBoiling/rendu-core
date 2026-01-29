#include "session_actor.h"
#include "server_actor.h"
#include "server_messages.h"
#include "common/log/logger.h"
#include <ctime>

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
    RENDU_LOG_INFO("SessionActor destroyed: {}", name());
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
    RENDU_LOG_INFO("SessionActor started: {}", name());
    // 开始接收数据
    start_receive();
}

void SessionActor::on_stop() {
    RENDU_LOG_INFO("SessionActor stopped: {}", name());

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

        // 处理每个完整消息
        for (const auto& message_data : messages) {
            // 解析 protobuf 消息
            protocol::ClientMessage client_msg;
            if (!client_msg.ParseFromArray(message_data.data(),
                                          static_cast<int>(message_data.size()))) {
                RENDU_LOG_ERROR("Failed to parse client message from user {}", user_id_);
                continue;
            }

            // 根据消息类型处理
            if (client_msg.has_login()) {
                on_login(client_msg.login());
            } else if (client_msg.has_chat()) {
                on_chat(client_msg.chat());
            } else {
                RENDU_LOG_WARN("Unknown message type from user {}", user_id_);
            }
        }
    } catch (const std::exception& e) {
        RENDU_LOG_ERROR("Error processing data from user {}: {}", user_id_, e.what());
    }
}

void SessionActor::on_login(const protocol::LoginRequest& request) {
    RENDU_LOG_INFO("Login request from {}: {}", request.username(), user_id_);

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
        RENDU_LOG_WARN("Chat message from unauthenticated user {}", user_id_);
        return;
    }

    RENDU_LOG_INFO("Chat from {}: {}", username_, message.content());

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
}

void SessionActor::send_to_client(const protocol::ServerMessage& msg) {
    if (!socket_) {
        return;
    }

    // 序列化 protobuf 消息
    std::string serialized;
    if (!msg.SerializeToString(&serialized)) {
        RENDU_LOG_ERROR("Failed to serialize server message for user {}", user_id_);
        return;
    }

    // 使用编码器添加长度前缀
    std::vector<byte> data(serialized.begin(), serialized.end());
    auto encoded_data = codec_->encode(data);

    // 发送编码后的数据
    socket_->async_send(encoded_data, [this, user_id = user_id_]
                        (const boost::system::error_code& ec, size_t bytes_sent) {
        if (ec) {
            RENDU_LOG_ERROR("Send error for user {}: {}", user_id, ec.message());
            // 停止 Actor
            if (system_) {
                system_->stop_actor(self());
            }
        }
    });
}

void SessionActor::send_raw_data(const std::vector<byte>& data) {
    if (!socket_) {
        return;
    }

    // 直接发送原始数据（假设已经编码）
    socket_->async_send(data, [this, user_id = user_id_]
                        (const boost::system::error_code& ec, size_t bytes_sent) {
        if (ec) {
            RENDU_LOG_ERROR("Send error for user {}: {}", user_id, ec.message());
            if (system_) {
                system_->stop_actor(self());
            }
        }
    });
}

void SessionActor::start_receive() {
    if (!socket_) {
        return;
    }

    socket_->async_receive(4096, [this](const boost::system::error_code& ec, std::vector<byte> data) {
        if (ec) {
            if (ec != boost::asio::error::operation_aborted) {
                RENDU_LOG_ERROR("Receive error for user {}: {}", user_id_, ec.message());
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

