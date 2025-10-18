// LogComponent.hpp
#pragma once

#include "core/ecs/Component.hpp"

#include "common/logger/LogLevel.hpp"

#include <chrono>
#include <string>

struct LogComponent : public core::Component {
    std::chrono::system_clock::time_point timestamp;
    logger::LogLevel level;
    std::string message;
    std::string category;

    LogComponent(logger::LogLevel lvl, std::string msg, std::string cat = "default")
        : timestamp(std::chrono::system_clock::now()),
          level(lvl),
          message(std::move(msg)),
          category(std::move(cat)) {}
};