#pragma once

#include "world.h"
#include <memory>
#include <string>

#include "common/threading/frame_rate_limiter.h"

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
            
            // 设置目标帧率
            void set_target_fps(float fps);
            
            // 获取当前目标帧率
            float get_target_fps() const;
            
            // 获取实际帧率
            float get_actual_fps() const;

        protected:
            // 注册系统
            virtual void register_systems() = 0;

            // 应用程序更新
            virtual void update(float delta_time) = 0;

            std::unique_ptr<World> world_;
            bool running_ = false;
            Threading::FrameRateLimiter frame_rate_limiter_; // 添加帧率限制器成员
            std::string name_;
            std::string _logger;
        };
    } // namespace Ecs

END_NAMESPACE_COMMON