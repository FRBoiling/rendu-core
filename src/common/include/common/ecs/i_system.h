#pragma once

#include "common/define.h"
#include <vector>
#include <string>
#include <typeinfo>
#include <functional>
#include <cstddef>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        class World;

        // Entity 类型前向声明
        using Entity = uint32_t;

        // 系统执行模式
        enum class SystemExecutionMode
        {
            PARALLEL,    // 支持并行执行（默认）
            SEQUENTIAL   // 必须串行执行
        };

        // 系统基类，所有系统都需要继承自这个类
        class ISystem
        {
        public:
            virtual ~ISystem() = default;

            // 配置阶段：系统配置（并行）
            virtual void configure(World* world) = 0;

            // 初始化阶段：系统初始化（并行）
            virtual void initialize() = 0;

            // 更新阶段（有序）：游戏逻辑（有序执行）
            virtual void update_sequential(float delta_time) = 0;

            // 更新阶段（并行）：同步/清理（并行执行）
            virtual void update_parallel(float delta_time) = 0;

            // 关闭阶段：系统关闭（并行）
            virtual void shutdown() = 0;

            // 清理阶段：资源清理（有序）
            virtual void cleanup() = 0;

            // 获取系统名称
            virtual std::string get_name() const = 0;

            // 获取系统依赖的其他系统
            virtual std::vector<std::string> get_dependencies() const = 0;

            // 获取系统执行模式（默认并行）
            virtual SystemExecutionMode get_execution_mode() const { return SystemExecutionMode::PARALLEL; }

        protected:
            World* world_ = nullptr;
            bool initialized_ = false;
        };

        // 系统基类模板 - 提供便捷的实体遍历和并行处理
        // 用户可以直接继承这个类获得便捷方法
        template <typename Derived>
        class System : public ISystem
        {
        public:
            // 配置阶段：默认保存 World 引用
            void configure(World* world) override { this->world_ = world; }

            // 初始化阶段：默认空实现
            void initialize() override {}

            // 更新阶段（有序）：默认调用 update_parallel
            void update_sequential(float delta_time) override
            {
                update_parallel(delta_time);
            }

            // 更新阶段（并行）：默认空实现，用户需要重写
            void update_parallel(float delta_time) override {}

            // 关闭阶段：默认空实现
            void shutdown() override {}

            // 清理阶段：默认空实现
            void cleanup() override {}

            // 获取系统名称（使用类名）
            std::string get_name() const override
            {
                return typeid(Derived).name();
            }

            // 默认无依赖
            std::vector<std::string> get_dependencies() const override
            {
                return {};
            }

            // 默认并行执行
            SystemExecutionMode get_execution_mode() const override
            {
                return SystemExecutionMode::PARALLEL;
            }

        protected:
            // 便捷方法：遍历包含指定组件的实体
            // 注意：这些方法的前向声明，实现在 world.h 中
            template <typename... ComponentTypes>
            void each(std::function<void(Entity, ComponentTypes*...)> func);

            // 并行遍历实体（自动分块）
            template <typename... ComponentTypes>
            void parallel_each(std::function<void(Entity, ComponentTypes*...)> func, std::size_t chunk_size = 256);

            // 手动控制任务分块的并行遍历
            template <typename... ComponentTypes>
            void parallel_for_range(std::function<void(Entity, std::size_t, ComponentTypes*...)> func,
                                   std::size_t chunk_size = 256);

            // 获取组件（便捷方法）
            template <typename ComponentType>
            ComponentType* get_component(Entity entity);

            template <typename ComponentType>
            const ComponentType* get_component(Entity entity) const;

            // 添加组件（便捷方法）
            template <typename ComponentType>
            void add_component(Entity entity, ComponentType&& component);

            // 移除组件
            template <typename ComponentType>
            void remove_component(Entity entity);

            // 检查是否有组件
            template <typename ComponentType>
            bool has_component(Entity entity) const;
        };

    } // namespace Ecs
END_NAMESPACE_COMMON
