#pragma once

#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        class World;

        // 系统基类，所有系统都需要继承自这个类
        class ISystem
        {
        public:
            virtual ~ISystem() = default;

            // 系统配置阶段
            virtual void configure(World* world) = 0;

            // 系统初始化阶段
            virtual void initialize() = 0;

            // 系统更新阶段
            virtual void update(float delta_time) = 0;

            // 系统关闭阶段
            virtual void shutdown() = 0;

            // 获取系统名称
            virtual std::string get_name() const = 0;

            // 获取系统依赖的其他系统
            virtual std::vector<std::string> get_dependencies() const = 0;

        protected:
            World* world_ = nullptr;
        };
    } // namespace Ecs

END_NAMESPACE_COMMON
