#ifndef COMMON_ECS_LOG_MACROS_H
#define COMMON_ECS_LOG_MACROS_H

#include "common/ecs/logger_system.h"

// 为ECS系统提供便捷的日志宏
#define ECS_LOG_DEBUG(...) common::Ecs::LoggerSystem::get_logger().debug(__VA_ARGS__)
#define ECS_LOG_INFO(...) common::Ecs::LoggerSystem::get_logger().info(__VA_ARGS__)
#define ECS_LOG_WARN(...) common::Ecs::LoggerSystem::get_logger().warn(__VA_ARGS__)
#define ECS_LOG_ERROR(...) common::Ecs::LoggerSystem::get_logger().error(__VA_ARGS__)
#define ECS_LOG_CRITICAL(...) common::Ecs::LoggerSystem::get_logger().critical(__VA_ARGS__)

#endif // COMMON_ECS_LOG_MACROS_H