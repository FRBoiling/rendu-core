//
// Created by FRee2 on 2025/10/18.
//

#ifndef RENDUCORE_LOGGINGSYSTEM_H
#define RENDUCORE_LOGGINGSYSTEM_H

// systems/logging_system.hpp
#pragma once
#include "ecs/ISystem.h"

#include <chrono>
#include <string>

// 日志组件
struct LogMessageComponent {
  std::string message;
  spdlog::level::level_enum level;
  std::string file;
  int line;
  std::string function;
  std::chrono::steady_clock::time_point timestamp;
};

// 日志标签
struct LogMessageTag {};

class LoggingSystem : public ISystem {
public:
  LoggingSystem(const std::string& log_level = "info",
                const std::string& log_file = "logs/server.log");
  ~LoggingSystem();

  void configure(entt::registry& registry) override;
  void update(entt::registry& registry, float delta_time) override;
  void initialize(entt::registry& registry) override;
  void shutdown(entt::registry& registry) override;
  std::string get_name() const override { return "LoggingSystem"; }

  // 日志接口
  void log(entt::registry& registry, spdlog::level::level_enum level,
           const std::string& message, const std::string& file = "",
           int line = 0, const std::string& function = "");

private:
  void setup_logger();
  void process_log_messages(entt::registry& registry);

  std::shared_ptr<spdlog::logger> logger_;
  std::string log_level_;
  std::string log_file_;
};

#endif // RENDUCORE_LOGGINGSYSTEM_H
