#include "core/actor/message.h"
#include <sstream>
#include <algorithm>

BEGIN_NAMESPACE_CORE

// 注册内置消息类型
static MessageRegistrar<EmptyMessage> g_EmptyMessage_registrar("EmptyMessage");

std::mutex MessageRegistry::registry_mutex_;

std::unordered_map<std::string, MessageFactory>& MessageRegistry::get_registry() {
    static std::unordered_map<std::string, MessageFactory> registry;
    return registry;
}

void MessageRegistry::register_type(const std::string& type, MessageFactory factory) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    get_registry()[type] = factory;
}

std::shared_ptr<Message> MessageRegistry::create(const std::string& type) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    auto it = get_registry().find(type);
    if (it != get_registry().end()) {
        return it->second();
    }
    return nullptr;
}

std::shared_ptr<Message> MessageRegistry::deserialize(const std::string& data) {
    // 格式: type|request_id|data
    size_t first_sep = data.find('|');
    if (first_sep == std::string::npos) {
        return nullptr;
    }

    std::string type = data.substr(0, first_sep);

    size_t second_sep = data.find('|', first_sep + 1);
    if (second_sep == std::string::npos) {
        return nullptr;
    }

    std::string request_id_str = data.substr(first_sep + 1, second_sep - first_sep - 1);
    std::string msg_data = data.substr(second_sep + 1);

    // 创建消息实例
    auto msg = create(type);
    if (!msg) {
        return nullptr;
    }

    // 设置 request_id
    try {
        uint64_t request_id = std::stoull(request_id_str);
        msg->set_request_id(request_id);
    } catch (...) {
        return nullptr;
    }

    // 反序列化消息数据
    msg->deserialize_data(msg_data);

    return msg;
}

bool MessageRegistry::is_registered(const std::string& type) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    return get_registry().find(type) != get_registry().end();
}

END_NAMESPACE_CORE
