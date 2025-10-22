#pragma once

#include "world.h"
#include <memory>
#include <string>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        class Application
        {
        public:
            Application();
            virtual ~Application();

            // 初始化应用程序
            bool initialize();

            // 运行应用程序主循环
            void run();

            // 关闭应用程序
            void shutdown();

            // 获取应用程序名称
            virtual std::string get_name() const { return "Rendu ECS Application"; }

        protected:
            // 注册系统
            virtual void register_systems() = 0;

            // 应用程序更新
            virtual void update(float delta_time) = 0;

            // 应用程序渲染
            virtual void render() = 0;

            std::unique_ptr<World> world_;
            bool running_ = false;
        };
    } // namespace Ecs

END_NAMESPACE_COMMON
