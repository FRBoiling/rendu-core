#pragma once

#include "i_system.h"
#include "registry.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

#include "common/threading/thread_pool_adapter.h"
#include "common/logging/log.h"

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 系统执行阶段
        enum class SystemPhase
        {
            CONFIGURE,          // 配置阶段：系统配置（并行）
            INITIALIZE,         // 初始化阶段：系统初始化（并行）
            UPDATE_SEQUENTIAL,  // 更新阶段：游戏逻辑（串行）
            UPDATE_PARALLEL,    // 更新后阶段：同步/清理（并行）
            SHUTDOWN,           // 关闭阶段：系统关闭（并行）
            CLEANUP             // 清理阶段：资源清理（串行）
        };

        // 阶段执行模式
        enum class PhaseExecutionMode
        {
            ORDERED,     // 有序执行：按依赖关系顺序执行（不分层级）
            PARALLEL     // 并行执行：按依赖层级分组，同层级可并行
        };

        // 系统管理器 - 负责管理系统的生命周期和执行
        class SystemManager
        {
        public:
            SystemManager(Threading::ThreadPoolAdapter& thread_pool)
                : thread_pool_(thread_pool)
            {
            }

            ~SystemManager()
            {
                shutdown_all();
                cleanup_all();
            }

            // 禁止拷贝和移动
            SystemManager(const SystemManager&) = delete;
            SystemManager& operator=(const SystemManager&) = delete;
            SystemManager(SystemManager&&) = delete;
            SystemManager& operator=(SystemManager&&) = delete;

            // ==================== 系统添加和获取 ====================

            // 添加系统
            template <typename SystemType, typename... Args>
            SystemType* add_system(Args&&... args)
            {
                static_assert(std::is_base_of<ISystem, SystemType>::value, "SystemType must derive from ISystem");

                auto system = std::make_unique<SystemType>(std::forward<Args>(args)...);
                auto system_ptr = system.get();

                std::string name = system->get_name();
                systems_[name] = std::move(system);
                system_type_map_[std::type_index(typeid(SystemType))] = system_ptr;

                RC_LOG_INFO("application", "System {} added", name);

                // 重新排序系统依赖
                sorted_systems_ = topological_sort_systems();

                return system_ptr;
            }

            // 按类型获取系统
            template <typename SystemType>
            SystemType* get_system()
            {
                auto it = system_type_map_.find(std::type_index(typeid(SystemType)));
                if (it != system_type_map_.end())
                {
                    return static_cast<SystemType*>(it->second);
                }
                return nullptr;
            }

            // 按名称获取系统
            ISystem* get_system(const std::string& name)
            {
                auto it = systems_.find(name);
                if (it != systems_.end())
                {
                    return it->second.get();
                }
                return nullptr;
            }

            // ==================== 系统阶段执行 ====================

            // 配置所有系统
            void configure_all(World* world);

            // 初始化所有系统
            void initialize_all();

            // 更新所有系统
            void update_all(float delta_time);

            // 关闭所有系统
            void shutdown_all();

            // 清理所有系统
            void cleanup_all();

            // 执行指定阶段的系统
            void execute_phase(World* world, SystemPhase phase, float delta_time = 0.0f);

        private:
            // ==================== 依赖排序 ====================

            // 拓扑排序系统
            std::vector<ISystem*> topological_sort_systems();

            // 按依赖层级分组系统（用于并行执行）
            std::vector<std::vector<ISystem*>> group_systems_by_level();

            // 获取阶段执行模式
            PhaseExecutionMode get_phase_mode(SystemPhase phase) const;

            // 获取阶段执行模式名称
            const char* get_phase_mode_name(PhaseExecutionMode mode) const;

            // 获取系统阶段名称
            const char* get_system_phase_name(SystemPhase phase) const;

            // ==================== 成员变量 ====================

            Threading::ThreadPoolAdapter& thread_pool_;
            std::unordered_map<std::string, std::unique_ptr<ISystem>> systems_;
            std::unordered_map<std::type_index, ISystem*> system_type_map_;
            std::vector<ISystem*> sorted_systems_;
            std::vector<std::vector<ISystem*>> leveled_systems_;  // 按层级分组的系统
        };

    } // namespace Ecs
END_NAMESPACE_COMMON
