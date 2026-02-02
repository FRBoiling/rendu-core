#pragma once

#include "core/define.h"
#include <string>
#include <cstdint>
#include <memory>
#include <functional>
#include <sstream>

BEGIN_NAMESPACE_CORE

/// Actor 引用，用于跨 Actor 消息传递
/// 支持本地和远程 Actor（支持序列化）
class ActorRef {
public:
    ActorRef() = default;

    ActorRef(const std::string& path, uint64_t id)
        : path_(path), actor_id_(id) {}

    /// 获取 Actor 路径（例如：/system/network）
    const std::string& path() const { return path_; }

    /// 获取 Actor ID
    uint64_t actor_id() const { return actor_id_; }

    /// 是否有效
    bool is_valid() const { return !path_.empty() && actor_id_ != 0; }

    /// 相等比较
    bool operator==(const ActorRef& other) const {
        return path_ == other.path_ && actor_id_ == other.actor_id_;
    }

    /// 不等比较
    bool operator!=(const ActorRef& other) const {
        return !(*this == other);
    }

    /// 哈希值（用于在 unordered_map 中作为键）
    size_t hash() const {
        return std::hash<std::string>()(path_) ^ std::hash<uint64_t>()(actor_id_);
    }

    /// 字符串表示
    std::string to_string() const {
        return path_ + "#" + std::to_string(actor_id_);
    }

    /// 无效引用
    static ActorRef invalid() {
        return ActorRef();
    }

    // ========== 序列化支持 ==========

    /// 序列化为字符串（用于跨进程通信）
    std::string serialize() const {
        std::ostringstream oss;
        oss << path_ << ":" << actor_id_;
        return oss.str();
    }

    /// 从字符串反序列化
    static ActorRef deserialize(const std::string& data) {
        size_t pos = data.rfind(':');
        if (pos == std::string::npos) {
            return invalid();
        }

        std::string path = data.substr(0, pos);
        std::string id_str = data.substr(pos + 1);

        // 验证 ID 部分不为空且全是数字
        if (id_str.empty()) {
            return invalid();
        }

        for (char c : id_str) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                return invalid();
            }
        }

        try {
            uint64_t id = std::stoull(id_str);
            return ActorRef(path, id);
        } catch (...) {
            return invalid();
        }
    }

    /// 验证序列化字符串格式
    static bool is_valid_serialized(const std::string& data) {
        size_t pos = data.rfind(':');
        if (pos == std::string::npos) {
            return false;
        }

        // 路径部分不能为空（pos 必须大于 0，至少有 "/:123" 这种格式）
        if (pos == 0) {
            return false;
        }

        std::string id_str = data.substr(pos + 1);
        if (id_str.empty()) {
            return false;
        }

        // 验证 ID 部分是数字
        for (char c : id_str) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                return false;
            }
        }

        return true;
    }

private:
    std::string path_;      // Actor 路径
    uint64_t actor_id_{0}; // Actor ID
};

/// ActorRef 哈希函数（用于 unordered_map）
struct ActorRefHash {
    size_t operator()(const ActorRef& ref) const {
        return ref.hash();
    }
};

END_NAMESPACE_CORE
