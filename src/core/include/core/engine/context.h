#pragma once

#include "core/define.h"
#include <memory>
#include <string>
#include <common/io/io_context.h>
#include <common/log/logger.h>
#include <common/event/event_bus.h>
#include <common/config/config.h>

BEGIN_NAMESPACE_CORE

class Context {
public:
    Context();
    ~Context();

    // 禁止拷贝和移动
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    // 访问子系统
    io::IoContext& io();
    const io::IoContext& io() const;

    log::Logger& logger();
    const log::Logger& logger() const;

    event::EventBus& event_bus();
    const event::EventBus& event_bus() const;

    config::Config& config();
    const config::Config& config() const;

    // 配置加载
    void load_config(const std::string& filepath);

private:
    std::unique_ptr<io::IoContext> io_;
    std::shared_ptr<log::Logger> logger_;
    std::unique_ptr<event::EventBus> event_bus_;
    config::Config config_;
};

END_NAMESPACE_CORE
