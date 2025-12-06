#pragma once

#include "core/define.h"
#include "common/ecs/i_system.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>

#include "common/asio/signal_set.h"

BEGIN_NAMESPACE_CORE
    class SignalSystem : public Ecs::ISystem
    {
    public:
        SignalSystem() = default;
        ~SignalSystem() override;

        // 配置阶段：系统配置（并行）
        void configure(Ecs::World* world) override;

        // 初始化阶段：系统初始化（并行）
        void initialize() override;

        // 更新阶段（有序）：游戏逻辑（有序执行）
        void update_sequential(float delta_time) override;

        // 更新阶段（并行）：同步/清理（并行执行）
        void update_parallel(float delta_time) override;

        // 关闭阶段：系统关闭（并行）
        void shutdown() override;

        // 清理阶段：资源清理（有序）
        void cleanup() override;

        // 系统信息
        std::string get_name() const override;
        std::vector<std::string> get_dependencies() const override;
        Ecs::SystemExecutionMode get_execution_mode() const override;

        // 信号处理回调注册
        using SignalCallback = std::function<void(int signal_number)>;
        void register_signal_handler(int signal, SignalCallback callback);
        void unregister_signal_handler(int signal);

    private:
        void handle_signal(const std::error_code& error, int signal_number);
        void register_default_signals();
        void start_async_wait();

        std::unique_ptr<Asio::SignalSet> signals_;
        std::unordered_map<int, std::vector<SignalCallback>> signal_handlers_;
        std::mutex handlers_mutex_;
    };

END_NAMESPACE_CORE
