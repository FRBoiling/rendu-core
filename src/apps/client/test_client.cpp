#include "test_client.h"
#include "common/log/logger.h"
#include <boost/asio.hpp>
#include <thread>

using namespace Rendu;
using namespace Rendu::net;

TestClient::TestClient(int client_id, io::IoContext& io)
    : client_id_(client_id), io_(io),
      codec_(std::make_unique<LengthPrefixCodec>()) {
    stats_.connect_time = std::chrono::steady_clock::now();
}

TestClient::~TestClient() {
    disconnect();
    stats_.disconnect_time = std::chrono::steady_clock::now();
}

void TestClient::connect(const std::string& host, uint16_t port, OnConnectedCallback callback) {
    socket_ = std::make_unique<TcpSocket>(io_);

    socket_->async_connect(host, port,
        [this, callback](const boost::system::error_code& ec) {
            if (ec) {
                if (on_error_) {
                    on_error_("Connect failed: " + ec.message());
                }
                return;
            }

            RENDU_LOG_INFO("Client {} connected to server", client_id_);

            start_receive();

            if (callback) {
                callback();
            }
        }
    );
}

void TestClient::login(const std::string& username) {
    protocol::ClientMessage client_msg;
    auto* login = client_msg.mutable_login();
    login->set_username(username);
    login->set_password("");  // 空密码

    std::string serialized;
    if (!client_msg.SerializeToString(&serialized)) {
        RENDU_LOG_ERROR("Failed to serialize login message");
        return;
    }

    std::vector<byte> data(serialized.begin(), serialized.end());
    send_raw(data);
}

void TestClient::send_chat(const std::string& content) {
    protocol::ClientMessage client_msg;
    auto* chat = client_msg.mutable_chat();
    chat->set_content(content);

    std::string serialized;
    if (!client_msg.SerializeToString(&serialized)) {
        RENDU_LOG_ERROR("Failed to serialize chat message");
        return;
    }

    std::vector<byte> data(serialized.begin(), serialized.end());
    send_raw(data);
}

void TestClient::start_chat_loop(int interval_ms, const std::string& content) {
    chat_loop_running_ = true;
    chat_interval_ = std::chrono::milliseconds(interval_ms);
    chat_content_ = content;

    auto send_loop = [this]() {
        while (chat_loop_running_ && is_connected()) {
            send_chat(chat_content_);
            stats_.messages_sent++;
            std::this_thread::sleep_for(chat_interval_);
        }
    };

    std::thread(send_loop).detach();
}

void TestClient::stop_chat_loop() {
    chat_loop_running_ = false;
}

void TestClient::disconnect() {
    stop_chat_loop();

    if (socket_) {
        socket_->close();
    }
}

bool TestClient::is_connected() const {
    return socket_ && socket_->is_connected();
}

int64_t TestClient::uptime_ms() const {
    auto end = stats_.disconnect_time;
    if (chat_loop_running_ || is_connected()) {
        end = std::chrono::steady_clock::now();
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - stats_.connect_time).count();
}

void TestClient::send_raw(const std::vector<byte>& data) {
    if (!socket_) {
        return;
    }

    auto encoded = codec_->encode(data);
    stats_.bytes_sent += encoded.size();

    socket_->async_send(encoded,
        [this](const boost::system::error_code& ec, size_t bytes_sent) {
            if (ec) {
                if (on_error_) {
                    on_error_("Send error: " + ec.message());
                }
            }
        }
    );
}

void TestClient::on_data_received(const std::vector<byte>& data) {
    try {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        receive_buffer_.insert(receive_buffer_.end(), data.begin(), data.end());

        auto messages = codec_->decode(receive_buffer_);

        for (const auto& message_data : messages) {
            protocol::ServerMessage server_msg;
            if (!server_msg.ParseFromArray(message_data.data(),
                                        static_cast<int>(message_data.size()))) {
                RENDU_LOG_ERROR("Failed to parse server message");
                continue;
            }

            stats_.messages_received++;
            stats_.bytes_received += message_data.size();

            if (on_message_) {
                on_message_(server_msg);
            }
        }
    } catch (const std::exception& e) {
        if (on_error_) {
            on_error_("Data receive error: " + std::string(e.what()));
        }
    }
}

void TestClient::start_receive() {
    if (!socket_) {
        return;
    }

    socket_->async_receive(4096,
        [this](const boost::system::error_code& ec, std::vector<byte> data) {
            if (ec) {
                if (ec != boost::asio::error::operation_aborted && on_error_) {
                    on_error_("Receive error: " + ec.message());
                }
                return;
            }

            on_data_received(data);

            if (is_connected()) {
                start_receive();
            }
        }
    );
}
