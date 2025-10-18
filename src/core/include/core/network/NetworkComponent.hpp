// NetworkComponent.hpp
#pragma once

#include "core/ecs/Component.hpp"
#include "common/platform/Socket.hpp"

#include <vector>
#include <mutex>
#include <atomic>
#include <string>

enum class ConnectionState {
    Disconnected,
    Connecting,
    Established,
    Closing
};


struct NetworkComponent : public core::Component {
    std::unique_ptr<platform::Socket> socket;  // 改为智能指针管理
    std::vector<uint8_t> recvBuffer;
    std::vector<uint8_t> sendBuffer;
    std::mutex bufferMutex;
    std::atomic<ConnectionState> state{ConnectionState::Disconnected};
    std::chrono::steady_clock::time_point lastActivity;  // 新增最后活动时间
    std::string remoteIP;
    uint16_t remotePort = 0;
    uint32_t retryCount = 0;
    uint32_t maxRetries = 3;  // 新增最大重试次数
    bool autoReconnect = true;

    // 新增方法
    bool isConnected() const {
        return state == ConnectionState::Established;
    }
};

