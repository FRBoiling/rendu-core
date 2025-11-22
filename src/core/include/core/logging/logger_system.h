#ifndef CORE_ECS_LOGGER_SYSTEM_H
#define CORE_ECS_LOGGER_SYSTEM_H

#include "core/define.h"
#include "common/ecs/i_system.h"
#include "common/logging/logger.h"

BEGIN_NAMESPACE_CORE
        // 日志系统类 - 继承自ISystem接口
        class RC_COMMON_API LoggerSystem : public Ecs::ISystem
        {
        public:
            LoggerSystem();
            virtual ~LoggerSystem() override = default;

            // ISystem接口实现
            void configure(Ecs::World* world) override;
            void initialize() override;
            void update(float delta_time) override;
            void shutdown() override;
            std::string get_name() const override;
            std::vector<std::string> get_dependencies() const override;

            // 获取全局日志器实例的便捷方法
            static Logging::Logger& get_logger();

        private:
            bool initialized_ = false;
            std::string application_name_ = "RenduCore";
        };

END_NAMESPACE_CORE

#endif // CORE_ECS_LOGGER_SYSTEM_H
