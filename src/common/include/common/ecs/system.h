//
// Created by boil on 2026/1/13.
//

#ifndef RENDU_ECS_SYSTEM_H
#define RENDU_ECS_SYSTEM_H

#include "common/define.h"
#include "registry_base.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // System - 系统基类
    // ============================================================================

    /**
     * @brief 系统(逻辑处理器)基类
     *
     * System 负责处理满足特定条件的实体,是 ECS 架构中的逻辑层。
     * 所有自定义系统都应该继承此类。
     */
    class RC_COMMON_API System
    {
    public:
        virtual ~System() = default;

        /**
         * @brief 更新系统
         * @param registry 注册中心引用
         * @param deltaTime 帧时间(秒)
         */
        virtual void update(RegistryBase& registry, float deltaTime) = 0;

        /**
         * @brief 获取系统名称
         * @return 系统名称
         */
        [[nodiscard]] virtual const char* name() const = 0;

        /**
         * @brief 获取系统优先级
         * @return 优先级,数值越小越先执行
         */
        [[nodiscard]] virtual int priority() const { return 0; }
    };

    // ============================================================================
    // SystemBuilder - 系统构建器
    // ============================================================================

    /**
     * @brief 系统构建器
     *
     * 用于方便地创建和配置系统。
     */
    class RC_COMMON_API SystemBuilder
    {
    public:
        SystemBuilder();
        ~SystemBuilder();

        /**
         * @brief 设置系统名称
         */
        SystemBuilder& setName(const char* name);

        /**
         * @brief 设置系统优先级
         */
        SystemBuilder& setPriority(int priority);

        /**
         * @brief 添加依赖的系统
         */
        SystemBuilder& dependsOn(const char* systemName);

        /**
         * @brief 构建 Lambda 系统
         * @tparam Func 函数类型,签名为 void(Registry&, float)
         */
        template <typename Func>
        std::unique_ptr<System> build(Func&& func) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };

    // ============================================================================
    // LambdaSystem - Lambda 表达式系统
    // ============================================================================

    /**
     * @brief Lambda 表达式系统
     *
     * 允许使用 Lambda 表达式快速创建系统,无需定义类。
     */
    class RC_COMMON_API LambdaSystem : public System
    {
    public:
        /**
         * @brief 函数类型
         */
        using UpdateFunc = std::function<void(RegistryBase&, float)>;

        /**
         * @brief 构造函数
         * @param updateFunc 更新函数
         * @param name 系统名称
         * @param priority 优先级
         */
        LambdaSystem(UpdateFunc updateFunc, const char* name, int priority = 0);

        void update(RegistryBase& registry, float deltaTime) override;
        [[nodiscard]] const char* name() const override;
        [[nodiscard]] int priority() const override;

    private:
        UpdateFunc m_updateFunc;
        std::string m_name;
        int m_priority;
    };

    // ============================================================================
    // SystemExecutor - 系统执行器
    // ============================================================================

    /**
     * @brief 系统执行器
     *
     * 管理所有系统的注册、排序和执行。
     * 支持依赖关系和优先级排序。
     */
    class RC_COMMON_API SystemExecutor
    {
    public:
        SystemExecutor();
        ~SystemExecutor();

        /**
         * @brief 添加系统
         * @param system 系统指针(执行器将接管所有权)
         */
        void addSystem(std::unique_ptr<System> system);

        /**
         * @brief 添加系统(使用构建器)
         * @param builder 系统构建器
         * @param updateFunc 更新函数
         */
        template <typename Func>
        void addSystem(const SystemBuilder& builder, Func&& updateFunc);

        /**
         * @brief 移除系统
         * @param name 系统名称
         * @return true 如果成功移除
         */
        bool removeSystem(const char* name);

        /**
         * @brief 获取系统
         * @param name 系统名称
         * @return 系统指针,如果不存在则返回 nullptr
         */
        [[nodiscard]] System* getSystem(const char* name);

        /**
         * @brief 执行所有系统
         * @param registry 注册中心引用
         * @param deltaTime 帧时间(秒)
         */
        void execute(RegistryBase& registry, float deltaTime);

        /**
         * @brief 清空所有系统
         */
        void clear();

        /**
         * @brief 获取系统数量
         */
        [[nodiscard]] size_t systemCount() const;

        /**
         * @brief 检查是否存在系统
         */
        [[nodiscard]] bool hasSystem(const char* name) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void sortSystems();
        bool checkDependencies() const;
    };

    // ============================================================================
    // 常用系统构建函数
    // ============================================================================

    /**
     * @brief 创建 Lambda 系统
     * @param updateFunc 更新函数
     * @param name 系统名称
     * @param priority 优先级
     * @return 系统指针
     */
    inline std::unique_ptr<System> makeSystem(
        std::function<void(RegistryBase&, float)> updateFunc,
        const char* name,
        int priority = 0)
    {
        return std::make_unique<LambdaSystem>(std::move(updateFunc), name, priority);
    }

    /**
     * @brief 创建针对特定组件组合的系统
     * @tparam ComponentTypes 要处理的组件类型
     * @param updateFunc 更新函数,签名为 void(Entity, Components&...)
     * @param name 系统名称
     * @param priority 优先级
     * @return 系统指针
     */
    template <typename... ComponentTypes, typename Func>
    std::unique_ptr<System> makeComponentSystem(
        Func&& updateFunc,
        const char* name,
        int priority = 0)
    {
        auto wrappedFunc = [updateFunc = std::forward<Func>(updateFunc)](RegistryBase& registry, float) {
            auto view = registry.view<ComponentTypes...>();
            view.each([&](Entity e) {
                // 临时存储组件指针
                std::tuple<ComponentTypes*...> components =
                    std::make_tuple(registry.tryGet<ComponentTypes>(e)...);

                // 调用用户函数
                std::apply([e, &updateFunc](auto*... comps) {
                    if ((comps && ...)) {
                        updateFunc(e, *comps...);
                    }
                }, components);
            });
        };

        return std::make_unique<LambdaSystem>(std::move(wrappedFunc), name, priority);
    }

END_NAMESPACE_ECS

#endif //RENDU_ECS_SYSTEM_H
