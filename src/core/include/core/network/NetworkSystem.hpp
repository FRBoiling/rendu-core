// NetworkSystem.hpp
#pragma once

#include "core/ecs/System.hpp"
#include "core/ecs/entity.hpp"

#include "common/platform/IOHandler.hpp"

#include "NetworkComponent.hpp"
#include <queue>
#include <functional>
#include <mutex>


class NetworkSystem : public core::System {
public:
    explicit NetworkSystem(core::World& world);
    ~NetworkSystem();

    void update(double deltaTime) override;

    void connect(core::Entity& entity,
                const std::string& ip,
                uint16_t port);

    void send(core::Entity& entity,
             const uint8_t* data,
             size_t length);

private:
    std::unique_ptr<platform::IOHandler> ioHandler_;
    std::queue<std::function<void()>> eventQueue_;
    std::mutex queueMutex_;

    void processIOEvents();
    void handleConnect(core::Entity& entity);
    void processRead(core::Entity& entity);
    void processWrite(core::Entity& entity);
    void handleError(core::Entity& entity, int errorCode);
};
