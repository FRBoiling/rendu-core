// NetworkSystem.cpp
#include "core/network/NetworkSystem.hpp"
#include "core/ecs/World.hpp"

NetworkSystem::NetworkSystem(core::World& world)
    : core::System(world)
{
    ioHandler_ = platform::IOHandler::create();
}

NetworkSystem::~NetworkSystem() {
    // 清理所有网络资源
    auto entities = world_.getEntitiesWith<NetworkComponent>();
    for (auto entity : entities) {
        if (auto nc = entity->getComponent<NetworkComponent>()) {
            nc->socket->close();
        }
    }
}

void NetworkSystem::update(double deltaTime) {
    processIOEvents();

    // 处理事件队列
    std::unique_lock<std::mutex> lock(queueMutex_);
    while (!eventQueue_.empty()) {
        auto task = std::move(eventQueue_.front());
        eventQueue_.pop();
        task();
    }
}

void NetworkSystem::processIOEvents() {
    ioHandler_->poll(0);

    ioHandler_->setEventCallback([this](auto fd, auto events) {
        std::unique_lock<std::mutex> lock(queueMutex_);

        auto entities = world_.getEntitiesWith<NetworkComponent>();
        for (auto entity : entities) {
            auto nc = entity->getComponent<NetworkComponent>();
            if (!nc || nc->socket->nativeHandle() != fd) continue;

            if (events & platform::IOHandler::Read) {
                eventQueue_.emplace([=] { processRead(*entity); });
            }
            if (events & platform::IOHandler::Write) {
                eventQueue_.emplace([=] { processWrite(*entity); });
            }
            if (events & platform::IOHandler::Error) {
                eventQueue_.emplace([=] { handleError(*entity, errno); });
            }
        }
    });
}

void NetworkSystem::connect(core::Entity& entity,
                          const std::string& ip,
                          uint16_t port) {
    auto& nc = entity.addComponent<NetworkComponent>();
    nc.remoteIP = ip;
    nc.remotePort = port;
    nc.state = ConnectionState::Connecting;

    eventQueue_.emplace([=, &entity] { handleConnect(entity); });
}

void NetworkSystem::handleConnect(core::Entity& entity) {
    auto nc = entity.getComponent<NetworkComponent>();
    if (!nc) return;

    try {
        auto socket = platform::Socket::create();
        socket->connect(nc->remoteIP, nc->remotePort);
        nc->socket = std::move(socket);
        nc->state = ConnectionState::Established;

        ioHandler_->addSocket(nc->socket->nativeHandle(),
            platform::IOHandler::Read | platform::IOHandler::Write);
    } catch (const std::system_error& e) {
        handleError(entity, e.code().value());
    }
}

void NetworkSystem::send(core::Entity& entity,
                        const uint8_t* data,
                        size_t length) {
    auto nc = entity.getComponent<NetworkComponent>();
    if (!nc) return;

    std::lock_guard<std::mutex> lock(nc->bufferMutex);
    nc->sendBuffer.insert(nc->sendBuffer.end(), data, data + length);
}

void NetworkSystem::processRead(core::Entity& entity) {
    auto nc = entity.getComponent<NetworkComponent>();
    if (!nc) return;

    try {
        uint8_t buffer[4096];
        ssize_t received = nc->socket->read(buffer, sizeof(buffer));

        if (received > 0) {
            std::lock_guard<std::mutex> lock(nc->bufferMutex);
            nc->recvBuffer.insert(nc->recvBuffer.end(), buffer, buffer + received);
        } else if (received == 0) {
            nc->state = ConnectionState::Closing;
        }
    } catch (const std::system_error& e) {
        handleError(entity, e.code().value());
    }
}

void NetworkSystem::processWrite(core::Entity& entity) {
    auto nc = entity.getComponent<NetworkComponent>();
    if (!nc || nc->sendBuffer.empty()) return;

    try {
        std::lock_guard<std::mutex> lock(nc->bufferMutex);
        ssize_t sent = nc->socket->write(nc->sendBuffer.data(), nc->sendBuffer.size());

        if (sent > 0) {
            nc->sendBuffer.erase(nc->sendBuffer.begin(),
                               nc->sendBuffer.begin() + sent);
        }
    } catch (const std::system_error& e) {
        handleError(entity, e.code().value());
    }
}

void NetworkSystem::handleError(core::Entity& entity, int errorCode) {
    auto nc = entity.getComponent<NetworkComponent>();
    if (!nc) return;

    nc->state = ConnectionState::Disconnected;
    nc->socket->close();

    if (nc->autoReconnect && nc->retryCount++ < 3) {
        connect(entity, nc->remoteIP, nc->remotePort);
    } else {
        world_.destroyEntity(entity.getID());
    }
}