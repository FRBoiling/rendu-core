#pragma once

#include "core/actor/message.h"
#include <string>

namespace server {

/// 会话登录消息
class SessionLoginMessage : public Rendu::Message {
public:
    SessionLoginMessage(int32_t user_id, const std::string& username)
        : Message(0), user_id_(user_id), username_(username) {}

    const char* get_type() const override { return "SessionLoginMessage"; }

    std::string serialize() const override {
        return "SessionLoginMessage|" + std::to_string(request_id_) + "|" +
               std::to_string(user_id_) + "|" + username_;
    }

    void deserialize_data(const std::string& data) override {
        // 不需要反序列化
    }

    int32_t user_id() const { return user_id_; }
    const std::string& username() const { return username_; }

private:
    int32_t user_id_;
    std::string username_;
};

/// 会话登出消息
class SessionLogoutMessage : public Rendu::Message {
public:
    explicit SessionLogoutMessage(int32_t user_id)
        : Message(0), user_id_(user_id) {}

    const char* get_type() const override { return "SessionLogoutMessage"; }

    std::string serialize() const override {
        return "SessionLogoutMessage|" + std::to_string(request_id_) + "|" +
               std::to_string(user_id_);
    }

    void deserialize_data(const std::string& data) override {
        // 不需要反序列化
    }

    int32_t user_id() const { return user_id_; }

private:
    int32_t user_id_;
};

/// 广播聊天消息
class BroadcastChatMessage : public Rendu::Message {
public:
    BroadcastChatMessage(int32_t from_user_id,
                         const std::string& from_username,
                         const std::string& content)
        : Message(0), from_user_id_(from_user_id),
          from_username_(from_username), content_(content) {}

    const char* get_type() const override { return "BroadcastChatMessage"; }

    std::string serialize() const override {
        return "BroadcastChatMessage|" + std::to_string(request_id_) + "|" +
               std::to_string(from_user_id_) + "|" + from_username_ + "|" + content_;
    }

    void deserialize_data(const std::string& data) override {
        // 不需要反序列化
    }

    int32_t from_user_id() const { return from_user_id_; }
    const std::string& from_username() const { return from_username_; }
    const std::string& content() const { return content_; }

private:
    int32_t from_user_id_;
    std::string from_username_;
    std::string content_;
};

/// 用户ID分配消息（响应）
class UserIdAssignedMessage : public Rendu::Message {
public:
    explicit UserIdAssignedMessage(int32_t user_id)
        : Message(0), user_id_(user_id) {}

    const char* get_type() const override { return "UserIdAssignedMessage"; }

    std::string serialize() const override {
        return "UserIdAssignedMessage|" + std::to_string(request_id_) + "|" +
               std::to_string(user_id_);
    }

    void deserialize_data(const std::string& data) override {
        // 不需要反序列化
    }

    int32_t user_id() const { return user_id_; }

private:
    int32_t user_id_;
};

/// 发送消息给客户端
class SendToClientMessage : public Rendu::Message {
public:
    SendToClientMessage(const std::vector<byte>& data)
        : Message(0), data_(data) {}

    const char* get_type() const override { return "SendToClientMessage"; }

    std::string serialize() const override {
        return "SendToClientMessage|" + std::to_string(request_id_) + "|" +
               std::to_string(data_.size());
    }

    void deserialize_data(const std::string& data) override {
        // 不需要反序列化
    }

    const std::vector<byte>& data() const { return data_; }

private:
    std::vector<byte> data_;
};

/// 获取新用户ID消息（请求）
class NewUserIdMessage : public Rendu::Message {
public:
    NewUserIdMessage() : Message(0) {}

    const char* get_type() const override { return "NewUserIdMessage"; }

    std::string serialize() const override {
        return "NewUserIdMessage|" + std::to_string(request_id_) + "|";
    }

    void deserialize_data(const std::string& data) override {
        // 不需要反序列化
    }
};

} // namespace server
