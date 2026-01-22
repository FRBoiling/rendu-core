#pragma once

#include "common/define.h"
#include <cstdint>
#include <chrono>
#include <string>

BEGIN_NAMESPACE_COMMON
namespace time {

/**
 * @brief 获取当前时间戳（毫秒）
 * @return 毫秒时间戳
 */
int64_t now_ms();

/**
 * @brief 获取当前时间戳（微秒）
 * @return 微秒时间戳
 */
int64_t now_us();

/**
 * @brief 格式化时间戳
 * @param timestamp_ms 毫秒时间戳
 * @param format 格式字符串（默认: "%Y-%m-%d %H:%M:%S"）
 * @return 格式化后的时间字符串
 */
std::string format_time(int64_t timestamp_ms, const char* format = "%Y-%m-%d %H:%M:%S");

/**
 * @brief 解析时间字符串
 * @param time_str 时间字符串
 * @param format 格式字符串（默认: "%Y-%m-%d %H:%M:%S"）
 * @return 毫秒时间戳
 */
int64_t parse_time(const std::string& time_str, const char* format = "%Y-%m-%d %H:%M:%S");

/**
 * @brief 毫秒级休眠
 * @param ms 休眠毫秒数
 */
void sleep_ms(uint32_t ms);

/**
 * @brief 高精度计时器
 */
class Timer {
public:
    Timer();

    /**
     * @brief 重置计时器
     */
    void reset();

    /**
     * @brief 获取经过的毫秒数
     * @return 毫秒数
     */
    int64_t elapsed_ms() const;

    /**
     * @brief 获取经过的微秒数
     * @return 微秒数
     */
    int64_t elapsed_us() const;

private:
    std::chrono::steady_clock::time_point start_;
};

} // namespace time
END_NAMESPACE_COMMON
