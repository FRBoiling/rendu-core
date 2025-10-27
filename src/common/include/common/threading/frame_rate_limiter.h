#pragma once

#include "common/define.h"
#include <chrono>

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        class FrameRateLimiter
        {
        public:
            // 构造函数，传入目标帧率（默认60fps）
            explicit FrameRateLimiter(float target_fps = 60.0f);
            
            // 设置目标帧率
            void set_target_fps(float target_fps);
            
            // 获取当前设置的目标帧率
            float get_target_fps() const;
            
            // 限制帧率，返回实际等待的毫秒数
            float limit();
            
            // 获取最后一帧的实际帧率
            float get_actual_fps() const;
            
        private:
            float target_fps_;
            float frame_time_ms_;
            
            using clock = std::chrono::high_resolution_clock;
            std::chrono::time_point<clock> last_frame_time_;
            std::chrono::time_point<clock> last_fps_calculation_time_;
            int frames_since_last_calculation_;
            float actual_fps_;
        };
    } // namespace Ecs
END_NAMESPACE_COMMON