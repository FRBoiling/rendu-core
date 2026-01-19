//
// Created by boil on 2026/1/15.
// ECS 信号/观察者系统 - 类似 ENTT
//

#ifndef RENDU_ECS_EVENTS_H
#define RENDU_ECS_EVENTS_H

#include "common/define.h"
#include "../ecs/entity.h"
#include "../ecs/registry_optimized.h"
#include <functional>
#include <vector>
#include <typeindex>
#include <memory>
#include <unordered_map>
#include <list>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // Signal - 信号系统 (类似 ENTT 的 basic_signal)
    // ============================================================================

    /**
     * @brief 信号实现 - 观察者模式的核心
     *
     * 类似 ENTT 的 basic_signal,支持:
     * - 连接/断开监听器
     * - 返回连接句柄用于自动管理生命周期
     * - 高性能的事件触发
     */
    template <typename Func>
    class Signal
    {
    public:
        using CallbackType = std::function<Func>;
        using ListenerId = size_t;

    private:
        /**
         * @brief 监听器包装器
         */
        struct Listener
        {
            ListenerId id;
            CallbackType callback;
        };

        std::list<Listener> m_listeners;
        ListenerId m_nextId = 1;

    public:
        Signal() = default;
        ~Signal() = default;

        Signal(const Signal&) = delete;
        Signal& operator=(const Signal&) = delete;
        Signal(Signal&&) noexcept = default;
        Signal& operator=(Signal&&) noexcept = default;

        /**
         * @brief 连接监听器
         * @param callback 回调函数
         * @return 监听器 ID,用于后续断开连接
         */
        [[nodiscard]] ListenerId connect(CallbackType callback)
        {
            ListenerId id = m_nextId++;
            m_listeners.push_back({id, std::move(callback)});
            return id;
        }

        /**
         * @brief 断开监听器
         * @param id 监听器 ID
         */
        void disconnect(ListenerId id)
        {
            m_listeners.remove_if([id](const Listener& listener) {
                return listener.id == id;
            });
        }

        /**
         * @brief 清空所有监听器
         */
        void clear()
        {
            m_listeners.clear();
            m_nextId = 1;
        }

        /**
         * @brief 获取监听器数量
         */
        [[nodiscard]] size_t size() const noexcept
        {
            return m_listeners.size();
        }

        /**
         * @brief 检查是否有监听器
         */
        [[nodiscard]] bool empty() const noexcept
        {
            return m_listeners.empty();
        }

        /**
         * @brief 触发信号 (变参模板)
         */
        template <typename... Args>
        void publish(Args&&... args) const
        {
            // 复制回调列表，避免在回调中修改监听器列表导致的迭代器失效（重入安全）
            std::vector<CallbackType> callbacks;
            callbacks.reserve(static_cast<size_t>(std::distance(m_listeners.begin(), m_listeners.end())));
            for (const auto& listener : m_listeners)
            {
                callbacks.push_back(listener.callback);
            }

            for (const auto& cb : callbacks)
            {
                cb(std::forward<Args>(args)...);
            }
        }

        /**
         * @brief 迭代器支持 - 用于手动遍历监听器
         */
        using iterator = typename std::list<Listener>::iterator;
        using const_iterator = typename std::list<Listener>::const_iterator;

        [[nodiscard]] iterator begin() { return m_listeners.begin(); }
        [[nodiscard]] iterator end() { return m_listeners.end(); }
        [[nodiscard]] const_iterator begin() const { return m_listeners.begin(); }
        [[nodiscard]] const_iterator end() const { return m_listeners.end(); }
    };

    // ============================================================================
    // Sink - 信号发射器 (类似 ENTT 的 sink)
    // ============================================================================

    /**
     * @brief 信号发射器 - 提供信号的安全接口
     *
     * 用于限制对信号的控制权限,只允许发布事件,不允许管理监听器。
     * 这提供了更好的封装性。
     */
    template <typename Func>
    class Sink
    {
    public:
        explicit Sink(Signal<Func>& signal) : m_signal(signal) {}

        /**
         * @brief 发布事件
         */
        template <typename... Args>
        void publish(Args&&... args) const
        {
            m_signal.publish(std::forward<Args>(args)...);
        }

        /**
         * @brief 获取监听器数量
         */
        [[nodiscard]] size_t size() const noexcept
        {
            return m_signal.size();
        }

        /**
         * @brief 检查是否有监听器
         */
        [[nodiscard]] bool empty() const noexcept
        {
            return m_signal.empty();
        }

    private:
        Signal<Func>& m_signal;
    };

    // ============================================================================
    // Connection - 连接管理器 (自动断开)
    // ============================================================================

    /**
     * @brief RAII 风格的连接管理器
     *
     * 析构时自动断开连接,避免手动管理。
     */
    template <typename Func>
    class Connection
    {
    public:
        Connection() = default;
        explicit Connection(Signal<Func>& signal, typename Signal<Func>::ListenerId id)
            : m_signal(&signal), m_id(id)
        {}

        ~Connection()
        {
            if (m_signal)
            {
                m_signal->disconnect(m_id);
            }
        }

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        Connection(Connection&& other) noexcept
            : m_signal(other.m_signal), m_id(other.m_id)
        {
            other.m_signal = nullptr;
        }

        Connection& operator=(Connection&& other) noexcept
        {
            if (this != &other)
            {
                if (m_signal)
                {
                    m_signal->disconnect(m_id);
                }
                m_signal = other.m_signal;
                m_id = other.m_id;
                other.m_signal = nullptr;
            }
            return *this;
        }

        /**
         * @brief 手动断开连接
         */
        void disconnect()
        {
            if (m_signal)
            {
                m_signal->disconnect(m_id);
                m_signal = nullptr;
            }
        }

        /**
         * @brief 检查连接是否有效
         */
        [[nodiscard]] bool valid() const noexcept
        {
            return m_signal != nullptr;
        }

    private:
        Signal<Func>* m_signal = nullptr;
        typename Signal<Func>::ListenerId m_id = 0;
    };

    // ============================================================================
    // 组件事件类型
    // ============================================================================

    /**
     * @brief 组件构造事件
     */
    template <typename ComponentType>
    struct OnConstructEvent
    {
        Entity entity;
        ComponentType* component;
    };

    /**
     * @brief 组件更新事件
     */
    template <typename ComponentType>
    struct OnUpdateEvent
    {
        Entity entity;
        ComponentType* component;
    };

    /**
     * @brief 组件销毁事件
     */
    template <typename ComponentType>
    struct OnDestroyEvent
    {
        Entity entity;
        ComponentType* component;
    };

    // ============================================================================
    // ComponentSink - 组件信号发射器
    // ============================================================================

    /**
     * @brief 组件级别的信号发射器
     *
     * 类似 ENTT 的 registry.on_construct<ComponentType>()
     */
    template <typename ComponentType>
    class ComponentSink
    {
    public:
        using ConstructFunc = std::function<void(Entity, ComponentType&)>;
        using UpdateFunc = std::function<void(Entity, ComponentType&)>;
        using DestroyFunc = std::function<void(Entity, ComponentType&)>;

        ComponentSink(
            Signal<void(Entity, ComponentType&)>& constructSignal,
            Signal<void(Entity, ComponentType&)>& updateSignal,
            Signal<void(Entity, ComponentType&)>& destroySignal
        ) : m_construct(constructSignal),
            m_update(updateSignal),
            m_destroy(destroySignal)
        {}

        /**
         * @brief 连接构造事件
         */
        [[nodiscard]] Connection<void(Entity, ComponentType&)> onConstruct(ConstructFunc callback)
        {
            auto id = m_construct.connect(std::move(callback));
            return Connection<void(Entity, ComponentType&)>(m_construct, id);
        }

        /**
         * @brief 连接更新事件
         */
        [[nodiscard]] Connection<void(Entity, ComponentType&)> onUpdate(UpdateFunc callback)
        {
            auto id = m_update.connect(std::move(callback));
            return Connection<void(Entity, ComponentType&)>(m_update, id);
        }

        /**
         * @brief 连接销毁事件
         */
        [[nodiscard]] Connection<void(Entity, ComponentType&)> onDestroy(DestroyFunc callback)
        {
            auto id = m_destroy.connect(std::move(callback));
            return Connection<void(Entity, ComponentType&)>(m_destroy, id);
        }

        /**
         * @brief 便捷方法：默认连接到构造事件
         */
        [[nodiscard]] Connection<void(Entity, ComponentType&)> connect(ConstructFunc callback)
        {
            return onConstruct(std::move(callback));
        }

        /**
         * @brief 断开所有监听器
         */
        void clear()
        {
            m_construct.clear();
            m_update.clear();
            m_destroy.clear();
        }

        /**
         * @brief 获取监听器数量
         */
        [[nodiscard]] size_t size() const noexcept
        {
            return m_construct.size() + m_update.size() + m_destroy.size();
        }

    private:
        Signal<void(Entity, ComponentType&)>& m_construct;
        Signal<void(Entity, ComponentType&)>& m_update;
        Signal<void(Entity, ComponentType&)>& m_destroy;
    };

    // ============================================================================
    // Dispatcher - 事件分发器
    // ============================================================================

    /**
     * @brief 全局事件分发器
     *
     * 管理所有类型的信号,类似于 ENTT 的 dispatcher。
     */
    class RC_COMMON_API Dispatcher
    {
    public:
        Dispatcher() = default;
        ~Dispatcher() = default;

        Dispatcher(const Dispatcher&) = delete;
        Dispatcher& operator=(const Dispatcher&) = delete;
        Dispatcher(Dispatcher&&) noexcept = default;
        Dispatcher& operator=(Dispatcher&&) noexcept = default;

        /**
         * @brief 获取或创建特定类型的信号
         */
        template <typename Func>
        Signal<Func>& signal()
        {
            std::type_index typeIndex(typeid(Signal<Func>));
            auto it = m_signals.find(typeIndex);

            if (it == m_signals.end())
            {
                auto sig = std::make_unique<Signal<Func>>();
                Signal<Func>* sigPtr = sig.get();
                m_signals[typeIndex] = std::move(sig);
                return *sigPtr;
            }

            return *static_cast<Signal<Func>*>(it->second.get());
        }

        /**
         * @brief 获取或创建特定类型的 Sink
         */
        template <typename Func>
        Sink<Func> sink()
        {
            return Sink<Func>(signal<Func>());
        }

        /**
         * @brief 清空所有信号
         */
        void clear()
        {
            m_signals.clear();
        }

    private:
        std::unordered_map<std::type_index, std::shared_ptr<void>> m_signals;
    };

END_NAMESPACE_ECS

#endif //RENDU_ECS_EVENTS_H
