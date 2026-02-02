#pragma once

#include "common/define.h"
#include <functional>
#include "event_define.h"

BEGIN_NAMESPACE_COMMON
    namespace event
    {
        /**
         * @brief 便捷的事件处理器包装器（模板）
         *
         * 提供类型安全的事件处理器
         * @tparam EventType 事件类型
         */
        template <typename EventType>
        class Handler
        {
        public:
            using Callback = std::function<void(const EventType&)>;

            explicit Handler(Callback cb)
                : callback_(std::move(cb))
            {
            }

            /**
             * @brief 函数调用操作符
             * @param event 事件对象
             */
            void operator()(const Event& event) const
            {
                auto* typed = dynamic_cast<const EventType*>(&event);
                if (typed)
                {
                    callback_(*typed);
                }
            }

            /**
             * @brief 转换为 EventHandler
             * @return EventHandler 函数对象
             */
            operator EventHandler() const
            {
                return [this](const Event& e) { (*this)(e); };
            }

        private:
            Callback callback_;
        };

        /**
         * @brief 创建类型安全的处理器
         * @tparam EventType 事件类型
         * @param callback 回调函数
         * @return EventHandler 对象
         */
        template <typename EventType>
        EventHandler make_handler(typename Handler<EventType>::Callback callback)
        {
            // 直接返回 lambda，避免 Handler 对象的生命周期问题
            return [cb = std::move(callback)](const Event& e)
            {
                auto* typed = dynamic_cast<const EventType*>(&e);
                if (typed)
                {
                    cb(*typed);
                }
            };
        }
    } // namespace event
END_NAMESPACE_COMMON
