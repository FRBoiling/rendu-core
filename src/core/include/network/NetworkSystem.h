//
// Created by FRee2 on 2025/10/18.
//

#ifndef RENDUCORE_NETWORKSYSTEM_H
#define RENDUCORE_NETWORKSYSTEM_H

#include "ecs/ISystem.h"
#include <chrono>
#include <entt/entt.hpp>
#include <vector>

// 网络组件
struct NetworkSessionComponent {
    uint32_t id;
    asio::ip::tcp::socket socket;
    asio::ip::tcp::endpoint endpoint;
    std::vector<uint8_t> read_buffer;
    std::vector<uint8_t> write_buffer;
    bool connected = false;
    std::chrono::steady_clock::time_point last_activity;
};

struct NetworkMessageComponent {
    uint32_t id;
    uint32_t session_id;
    std::vector<uint8_t> data;
    std::chrono::steady_clock::time_point timestamp;
};

// 网络标签组件
struct NetworkSessionTag {};
struct NetworkMessageTag {};

// 网络事件组件
struct ClientConnectedEvent {
    entt::entity session_entity;
};

struct ClientDisconnectedEvent {
    entt::entity session_entity;
};

struct MessageReceivedEvent {
    entt::entity message_entity;
    entt::entity session_entity;
};

class NetworkSystem : public ISystem {
public:
    NetworkSystem(asio::io_context& io_context, uint16_t port);
    ~NetworkSystem();

    void configure(entt::registry& registry) override;
    void update(entt::registry& registry, float delta_time) override;
    void initialize(entt::registry& registry) override;
    void shutdown(entt::registry& registry) override;
    std::string get_name() const override { return "NetworkSystem"; }

    void send_message(entt::registry& registry, entt::entity session_entity,
                     const std::vector<uint8_t>& data);

private:
    void start_accept(entt::registry& registry);
    void handle_accept(entt::registry& registry, std::error_code ec,
                      asio::ip::tcp::socket socket);
    void start_read(entt::registry& registry, entt::entity session_entity);
    void handle_read(entt::registry& registry, entt::entity session_entity,
                    std::error_code ec, size_t bytes_read);

    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    uint16_t port_;
    uint32_t next_session_id_ = 1;
    uint32_t next_message_id_ = 1;
};

#endif // RENDUCORE_NETWORKSYSTEM_H
