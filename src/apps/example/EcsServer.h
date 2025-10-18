//
// Created by FRee2 on 2025/10/18.
//

#ifndef RENDUCORE_ECSSERVER_H
#define RENDUCORE_ECSSERVER_H
#include <cstdint>
#include <memory>

// ECS 服务器主类
class EcsServer {
public:
  EcsServer();
  ~EcsServer();

  bool initialize(uint16_t port = 8080);
  void run();
  void stop();

  // 系统管理
  template<typename SystemType, typename... Args>
  void add_system(Args&&... args) {
    auto system = std::make_unique<SystemType>(std::forward<Args>(args)...);
    systems_.push_back(std::move(system));
  }

  template<typename SystemType>
  SystemType* get_system() {
    for (auto& system : systems_) {
      if (auto ptr = dynamic_cast<SystemType*>(system.get())) {
        return ptr;
      }
    }
    return nullptr;
  }

  entt::registry& get_registry() { return registry_; }
  asio::io_context& get_io_context() { return io_context_; }

private:
  void setup_core_components();
  void setup_core_systems();
  void main_loop();

  entt::registry registry_;
  asio::io_context io_context_;
  std::unique_ptr<asio::io_context::work> work_guard_;

  std::vector<std::unique_ptr<ISystem>> systems_;
  std::vector<std::thread> worker_threads_;
  std::atomic<bool> running_{false};
};

#endif // RENDUCORE_ECSSERVER_H
