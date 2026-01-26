//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include "core/engine/context.h"
#include <common/config/loader.h>

BEGIN_NAMESPACE_CORE

Context::Context()
    : io_(std::make_unique<io::IoContext>(4)) // 默认 4 个线程
    , logger_(std::make_shared<log::Logger>("RenduCore", *io_))
    , event_bus_(std::make_unique<event::EventBus>(*io_))
    , config_() {
    // 初始化配置为空
}

Context::~Context() {
    if (event_bus_) {
        event_bus_->clear();
    }
}

io::IoContext& Context::io() {
    return *io_;
}

const io::IoContext& Context::io() const {
    return *io_;
}

log::Logger& Context::logger() {
    return *logger_;
}

const log::Logger& Context::logger() const {
    return *logger_;
}

event::EventBus& Context::event_bus() {
    return *event_bus_;
}

const event::EventBus& Context::event_bus() const {
    return *event_bus_;
}

config::Config& Context::config() {
    return config_;
}

const config::Config& Context::config() const {
    return config_;
}

void Context::load_config(const std::string& filepath) {
    // 简化实现，暂时只记录日志
    logger().info("尝试加载配置文件: " + filepath);
    // 实际加载逻辑可以后续添加
}

END_NAMESPACE_CORE
