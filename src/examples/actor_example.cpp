#include "core/actor/actor.h"
#include "example_messages.h"

#include <chrono>
#include <iostream>
#include <thread>

using namespace Rendu;

/// Ping Actor - 发送 ping 消息
class PingActor : public Actor {
public:
    PingActor(std::string name, ActorRef pong, ActorSystem* system)
        : Actor(name), pong_(pong), system_(system), ping_count_(0) {}

    void receive(std::shared_ptr<Message> msg) override {
        const auto* text_msg = dynamic_cast<const TextMessage*>(msg.get());
        if (text_msg) {
            std::cout << "[Ping] 收到消息: " << text_msg->text() << std::endl;

            if (text_msg->text() == "pong") {
                ping_count_++;
                std::cout << "[Ping] 已收到 " << ping_count_ << " 次 pong" << std::endl;

                if (ping_count_ < 5) {
                    // 继续发送 ping
                    system_->tell(pong_, std::make_shared<TextMessage>("ping"));
                } else {
                    // 发送停止消息
                    system_->tell(pong_, std::make_shared<StopMessage>());
                    std::cout << "[Ping] 完成 5 次 ping-pong，结束通信" << std::endl;
                }
            }
        }

        const auto* stop_msg = dynamic_cast<const StopMessage*>(msg.get());
        if (stop_msg) {
            std::cout << "[Ping] 收到停止信号" << std::endl;
        }
    }

private:
    ActorRef pong_;
    ActorSystem* system_;
    int32_t ping_count_{0};
};

/// Pong Actor - 响应 ping 消息
class PongActor : public Actor {
public:
    explicit PongActor(std::string name) : Actor(name), pong_count_(0) {}

    void receive(std::shared_ptr<Message> msg) override {
        const auto* text_msg = dynamic_cast<const TextMessage*>(msg.get());
        if (text_msg) {
            std::cout << "[Pong] 收到消息: " << text_msg->text() << std::endl;

            if (text_msg->text() == "ping") {
                pong_count_++;
                std::cout << "[Pong] 已响应 " << pong_count_ << " 次 ping" << std::endl;
                // Pong 不需要回复，实际应用中可以根据需要处理
            }
        }

        const auto* stop_msg = dynamic_cast<const StopMessage*>(msg.get());
        if (stop_msg) {
            std::cout << "[Pong] 收到停止信号，总共处理了 " << pong_count_ << " 次 ping" << std::endl;
        }
    }

    void on_start() override {
        std::cout << "[Pong] Actor 已启动" << std::endl;
    }

    void on_stop() override {
        std::cout << "[Pong] Actor 已停止" << std::endl;
    }

private:
    int32_t pong_count_{0};
};

/// 打印 Actor - 演示计数消息处理
class PrinterActor : public Actor {
public:
    explicit PrinterActor(std::string name) : Actor(name), total_count_(0) {}

    void receive(std::shared_ptr<Message> msg) override {
        const auto* counter_msg = dynamic_cast<const CounterMessage*>(msg.get());
        if (counter_msg) {
            total_count_ += counter_msg->count();
            std::cout << "[Printer] 累计计数: " << total_count_ << std::endl;
        }
    }

private:
    int64_t total_count_{0};
};

int main() {
    std::cout << "========== Actor 示例程序 ==========" << std::endl;
    std::cout << "演示 Tell 模式（异步消息传递）" << std::endl;
    std::cout << std::endl;

    // 创建 Actor 系统（4 个工作线程）
    ActorSystem system(4);
    system.start();
    std::cout << "[系统] Actor 系统已启动" << std::endl;

    // 创建 Pong Actor
    auto pong = system.create_actor("pong", []() {
        return std::make_unique<PongActor>("pong");
    });
    std::cout << "[系统] 已创建 Pong Actor: " << pong.to_string() << std::endl;

    // 创建 Ping Actor，持有 Pong 的引用
    auto ping = system.create_actor("ping", [&system, &pong]() {
        return std::make_unique<PingActor>("ping", pong, &system);
    });
    std::cout << "[系统] 已创建 Ping Actor: " << ping.to_string() << std::endl;

    // 创建计数 Actor
    auto printer = system.create_actor("printer", []() {
        return std::make_unique<PrinterActor>("printer");
    });
    std::cout << "[系统] 已创建 Printer Actor: " << printer.to_string() << std::endl;

    std::cout << std::endl;
    std::cout << "=== 开始 Ping-Pong 通信 ===" << std::endl;

    // Ping 发起第一个 ping
    system.tell(ping, std::make_shared<TextMessage>("start"));

    // 发送一些计数消息给 Printer
    system.tell(printer, std::make_shared<CounterMessage>(10));
    system.tell(printer, std::make_shared<CounterMessage>(20));
    system.tell(printer, std::make_shared<CounterMessage>(30));

    // 等待消息处理
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << std::endl;
    std::cout << "=== 演示 Ask 模式（同步等待响应）===" << std::endl;

    // Ask 模式：发送消息并等待响应（超时 1 秒）
    auto request = std::make_shared<TextMessage>("hello");
    auto response = system.ask(pong, request, 1000);

    if (response) {
        if (auto* text_resp = dynamic_cast<const TextMessage*>(response.get())) {
            std::cout << "[系统] 收到响应: " << text_resp->text() << std::endl;
        } else {
            std::cout << "[系统] 收到响应（类型: " << response->get_type() << "）" << std::endl;
        }
    } else {
        std::cout << "[系统] 请求超时" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== 清理 ===" << std::endl;
    std::cout << "[系统] 当前 Actor 数量: " << system.actor_count() << std::endl;

    // 停止 Actor 系统
    system.stop();
    std::cout << "[系统] Actor 系统已停止" << std::endl;

    std::cout << std::endl;
    std::cout << "========== 示例程序结束 ==========" << std::endl;

    return 0;
}
