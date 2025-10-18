//
// Created by FRee2 on 2025/10/18.
//

// systems/logging_system.cpp
#include "logging/LoggingSystem.h"
#include <filesystem>

LoggingSystem::LoggingSystem(const std::string& log_level, const std::string& log_file)
    : log_level_(log_level), log_file_(log_file) {
}

LoggingSystem::~LoggingSystem() {
}

void LoggingSystem::configure(entt::registry& registry) {
    // 注册日志消息处理
    registry.on_construct<LogMessageComponent>().connect<[](entt::registry& reg, entt::entity entity) {
        // 可以在这里添加额外的日志处理逻辑
    }>();
}

void LoggingSystem::update(entt::registry& registry, float delta_time) {
    process_log_messages(registry);
}

void LoggingSystem::initialize(entt::registry& registry) {
    setup_logger();
    spdlog::info("Logging system initialized");
}

void LoggingSystem::shutdown(entt::registry& registry) {
    if (logger_) {
        logger_->flush();
        spdlog::drop(logger_->name());
    }
    spdlog::info("Logging system shutdown");
}

void LoggingSystem::log(entt::registry& registry, spdlog::level::level_enum level,
                       const std::string& message, const std::string& file,
                       int line, const std::string& function) {
    // 创建日志消息实体
    auto entity = registry.create();
    auto& log_msg = registry.emplace<LogMessageComponent>(entity);
    auto& tag = registry.emplace<LogMessageTag>(entity);

    log_msg.message = message;
    log_msg.level = level;
    log_msg.file = file;
    log_msg.line = line;
    log_msg.function = function;
    log_msg.timestamp = std::chrono::steady_clock::now();
}

void LoggingSystem::setup_logger() {
    // 创建日志目录
    std::filesystem::create_directories("logs");

    // 创建sinks
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        log_file_, 10 * 1024 * 1024, 5);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");

    // 创建logger
    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    logger_ = std::make_shared<spdlog::logger>("ecs_server", sinks.begin(), sinks.end());

    // 设置日志级别
    if (log_level_ == "trace") logger_->set_level(spdlog::level::trace);
    else if (log_level_ == "debug") logger_->set_level(spdlog::level::debug);
    else if (log_level_ == "info") logger_->set_level(spdlog::level::info);
    else if (log_level_ == "warn") logger_->set_level(spdlog::level::warn);
    else if (log_level_ == "error") logger_->set_level(spdlog::level::error);
    else if (log_level_ == "critical") logger_->set_level(spdlog::level::critical);

    logger_->flush_on(spdlog::level::warn);
    spdlog::register_logger(logger_);
}

void LoggingSystem::process_log_messages(entt::registry& registry) {
    auto view = registry.view<LogMessageComponent, LogMessageTag>();

    for (auto entity : view) {
        auto& log_msg = view.get<LogMessageComponent>(entity);

        // 使用spdlog输出
        logger_->log(log_msg.level, "{}", log_msg.message);

        // 处理完成后销毁实体
        registry.destroy(entity);
    }
}
