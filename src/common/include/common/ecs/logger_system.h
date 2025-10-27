#ifndef COMMON_ECS_LOGGER_SYSTEM_H
#define COMMON_ECS_LOGGER_SYSTEM_H

#include "common/ecs/i_system.h"
#include "common/logging/logger.h"

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 日志系统类 - 继承自ISystem接口
        class RC_COMMON_API LoggerSystem : public ISystem
        {
        public:
            LoggerSystem();
            virtual ~LoggerSystem() override = default;
            
            // ISystem接口实现
            void configure(World* world) override;
            void initialize() override;
            void update(float delta_time) override;
            void shutdown() override;
            std::string get_name() const override;
            std::vector<std::string> get_dependencies() const override;
            
            // 获取全局日志器实例的便捷方法
            static Logger& get_logger();
            
        private:
            bool initialized_ = false;
            std::string application_name_ = "RenduCore";
        };
    }
END_NAMESPACE_COMMON

#endif // COMMON_ECS_LOGGER_SYSTEM_H