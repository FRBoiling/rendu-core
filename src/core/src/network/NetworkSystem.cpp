//
// Created by FRee2 on 2025/10/18.
//

#include "network/NetworkSystem.h"
#include <spdlog/spdlog.h>

NetworkSystem::NetworkSystem(asio::io_context& io_context, uint16_t port)
    : io_context_(io_context), acceptor_(io_context), port_(port) {
}

NetworkSystem::~NetworkSystem() {
}

void NetworkSystem::configure(entt::registry& registry) {
    // 注册网络相关组件
    registry.on_construct<NetworkSessionComponent>().connect<[](entt::registry& reg, entt::entity entity) {
        auto& session = reg.get<NetworkSessionComponent>(entity);
        spdlog::info("Network session created: {}", session.id);
    }>();

    registry.on_destroy<NetworkSessionComponent>().connect<[](entt::registry& reg, entt::entity entity) {
        if (reg.valid(entity) && reg.all_of<NetworkSessionComponent>(entity)) {
            auto& session = reg.get<NetworkSessionComponent>(entity);
            spdlog::info("Network session destroyed: {}", session.id);
        }
    }>();
}

void NetworkSystem::update(entt::registry& registry, float delta_time) {
    // 清理超时会话
    auto now = std::chrono::steady_clock::now();
    auto view = registry.view<NetworkSessionComponent>();

    for (auto entity : view) {
        auto& session = view.get<NetworkSessionComponent>(entity);
        auto duration = now - session.last_activity;

        if (duration > std::chrono::minutes(5)) {
            spdlog::info("Session timeout: {}", session.id);

            // 断开连接
            if (session.socket.is_open()) {
                session.socket.close();
            }
            registry.destroy(entity);
        }
    }
}

void NetworkSystem::initialize(entt::registry& registry) {
    try {
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port_);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        start_accept(registry);
        spdlog::info("Network system initialized on port {}", port_);

    } catch (const std::exception& e) {
        spdlog::error("Failed to initialize network system: {}", e.what());
        throw;
    }
}

void NetworkSystem::shutdown(entt::registry& registry) {
    if (acceptor_.is_open()) {
        acceptor_.close();
    }

    // 断开所有连接
    auto view = registry.view<NetworkSessionComponent>();
    for (auto entity : view) {
        auto& session = view.get<NetworkSessionComponent>(entity);
        if (session.socket.is_open()) {
            session.socket.close();
        }
    }

    spdlog::info("Network system shutdown");
}

void NetworkSystem::start_accept(entt::registry& registry) {
    acceptor_.async_accept([this, &registry](std::error_code ec, asio::ip::tcp::socket socket) {
        handle_accept(registry, ec, std::move(socket));
    });
}

void NetworkSystem::handle_accept(entt::registry& registry, std::error_code ec,
                                asio::ip::tcp::socket socket) {
    if (!ec) {
        // 创建会话实体
        auto entity = registry.create();
        auto& session = registry.emplace<NetworkSessionComponent>(entity);
        auto& tag = registry.emplace<NetworkSessionTag>(entity);

        session.id = next_session_id_++;
        session.socket = std::move(socket);
        session.endpoint = session.socket.remote_endpoint();
        session.connected = true;
        session.last_activity = std::chrono::steady_clock::now();

        spdlog::info("New connection from: {} (session: {})",
                    session.endpoint.address().to_string(), session.id);

        // 开始读取数据
        start_read(registry, entity);
    } else {
        spdlog::error("Accept error: {}", ec.message());
    }

    // 继续接受新连接
    start_accept(registry);
}

void NetworkSystem::start_read(entt::registry& registry, entt::entity session_entity) {
    if (!registry.valid(session_entity) || !registry.all_of<NetworkSessionComponent>(session_entity)) {
        return;
    }

    auto& session = registry.get<NetworkSessionComponent>(session_entity);
    session.read_buffer.resize(4096);

    session.socket.async_read_some(
        asio::buffer(session.read_buffer),
        [this, &registry, session_entity](std::error_code ec, size_t bytes_read) {
            handle_read(registry, session_entity, ec, bytes_read);
        }
    );
}

void NetworkSystem::handle_read(entt::registry& registry, entt::entity session_entity,
                               std::error_code ec, size_t bytes_read) {
    if (ec) {
        if (ec != asio::error::eof) {
            spdlog::warn("Read error: {}", ec.message());
        }

        // 断开连接
        if (registry.valid(session_entity) && registry.all_of<NetworkSessionComponent>(session_entity)) {
            auto& session = registry.get<NetworkSessionComponent>(session_entity);
            if (session.socket.is_open()) {
                session.socket.close();
            }
            registry.destroy(session_entity);
        }
        return;
    }

    // 更新活动时间
    auto& session = registry.get<NetworkSessionComponent>(session_entity);
    session.last_activity = std::chrono::steady_clock::now();

    // 创建消息实体
    auto message_entity = registry.create();
    auto& message = registry.emplace<NetworkMessageComponent>(message_entity);
    auto& message_tag = registry.emplace<NetworkMessageTag>(message_entity);

    message.id = next_message_id_++;
    message.session_id = session.id;
    message.data.assign(session.read_buffer.begin(), session.read_buffer.begin() + bytes_read);
    message.timestamp = std::chrono::steady_clock::now();

    spdlog::debug("Received {} bytes from session {}", bytes_read, session.id);

    // 触发消息接收事件（通过组件）
    registry.ctx().emplace<MessageReceivedEvent>(message_entity, session_entity);

    // 继续读取
    start_read(registry, session_entity);
}

void NetworkSystem::send_message(entt::registry& registry, entt::entity session_entity,
                               const std::vector<uint8_t>& data) {
    if (!registry.valid(session_entity) || !registry.all_of<NetworkSessionComponent>(session_entity)) {
        return;
    }

    auto& session = registry.get<NetworkSessionComponent>(session_entity);

    asio::async_write(
        session.socket,
        asio::buffer(data),
        [&registry, session_entity](std::error_code ec, size_t bytes_written) {
            if (ec) {
                spdlog::warn("Write error: {}", ec.message());

                // 断开连接
                if (registry.valid(session_entity) && registry.all_of<NetworkSessionComponent>(session_entity)) {
                    auto& session = registry.get<NetworkSessionComponent>(session_entity);
                    if (session.socket.is_open()) {
                        session.socket.close();
                    }
                    registry.destroy(session_entity);
                }
            } else {
                spdlog::debug("Sent {} bytes to session {}", bytes_written,
                            registry.get<NetworkSessionComponent>(session_entity).id);
            }
        }
    );
}