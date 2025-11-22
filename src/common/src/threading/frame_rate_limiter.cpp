#include "common/threading/frame_rate_limiter.h"
#include <thread>
BEGIN_NAMESPACE_COMMON

using namespace Threading;

FrameRateLimiter::FrameRateLimiter(float target_fps)
    : target_fps_(target_fps),
      frame_time_ms_(1000.0f / target_fps),
      last_frame_time_(clock::now()),
      last_fps_calculation_time_(clock::now()),
      frames_since_last_calculation_(0),
      actual_fps_(0.0f)
{}

void FrameRateLimiter::set_target_fps(float target_fps)
{
    if (target_fps > 0.0f)
    {
        target_fps_ = target_fps;
        frame_time_ms_ = 1000.0f / target_fps;
    }
}

float FrameRateLimiter::get_target_fps() const
{
    return target_fps_;
}

float FrameRateLimiter::limit()
{
    // 计算自上一帧以来的时间
    auto current_time = clock::now();
    auto elapsed_time = std::chrono::duration<float, std::milli>(current_time - last_frame_time_).count();
    
    // 计算需要休眠的时间
    float sleep_time_ms = frame_time_ms_ - elapsed_time;
    
    // 只在需要时休眠（避免负的休眠时间）
    if (sleep_time_ms > 0.0f)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(sleep_time_ms * 1000)));
    }
    
    // 更新最后一帧的时间
    last_frame_time_ = clock::now();
    
    // 计算实际帧率（每秒更新一次）
    frames_since_last_calculation_++;
    auto time_since_last_calculation = std::chrono::duration<float, std::milli>(last_frame_time_ - last_fps_calculation_time_).count();
    
    if (time_since_last_calculation >= 1000.0f) // 每秒更新一次帧率
    {
        actual_fps_ = (frames_since_last_calculation_ * 1000.0f) / time_since_last_calculation;
        last_fps_calculation_time_ = last_frame_time_;
        frames_since_last_calculation_ = 0;
    }
    
    return sleep_time_ms > 0.0f ? sleep_time_ms : 0.0f;
}

float FrameRateLimiter::get_actual_fps() const
{
    return actual_fps_;
}

END_NAMESPACE_COMMON