#pragma once

#include "common/define.h"
#include "common/config/config.h"
#include "common/io/timer.h"
#include <functional>
#include <string>
#include <vector>
#include <atomic>
#include <chrono>
#include <filesystem>

BEGIN_NAMESPACE_COMMON
using namespace Rendu::io;

namespace config {

/**
 * @brief 配置文件监控器
 *
 * 使用定时器轮询文件修改时间，支持配置热更新。
 * 当检测到文件修改时，会重新加载配置并触发回调。
 */
class ConfigWatcher {
public:
    /**
     * @brief 配置变更回调函数类型
     * @param config 新加载的配置对象
     */
    using Callback = std::function<void(const Config&)>;

    /**
     * @brief 构造函数
     * @param io_context IoContext 引用，用于创建定时器
     * @param file_path 配置文件路径
     * @param check_interval 检查间隔（默认 1 秒）
     */
    explicit ConfigWatcher(
        IoContext& io_context,
        const std::string& file_path,
        std::chrono::milliseconds check_interval = std::chrono::seconds(1));

    /**
     * @brief 析构函数，自动停止监控
     */
    ~ConfigWatcher();

    /**
     * @brief 禁止拷贝构造
     */
    ConfigWatcher(const ConfigWatcher&) = delete;
    ConfigWatcher& operator=(const ConfigWatcher&) = delete;

    /**
     * @brief 移动构造
     */
    ConfigWatcher(ConfigWatcher&& other) noexcept;

    /**
     * @brief 移动赋值
     * @note 由于 io_context_ 是引用类型，移动赋值操作不安全，已禁用
     */
    ConfigWatcher& operator=(ConfigWatcher&& other) noexcept = delete;

    /**
     * @brief 开始监控
     */
    void start();

    /**
     * @brief 停止监控
     */
    void stop();

    /**
     * @brief 注册配置变更回调
     * @param callback 回调函数
     */
    void on_change(Callback callback);

    /**
     * @brief 手动触发配置重载
     * @return Result<Config> 加载结果
     */
    Result<Config> reload();

    /**
     * @brief 检查是否正在运行
     * @return 是否运行中
     */
    bool is_running() const;

    /**
     * @brief 获取配置文件路径
     * @return 文件路径
     */
    const std::string& file_path() const;

    /**
     * @brief 获取当前配置
     * @return 配置引用
     */
    const Config& config() const;

private:
    /**
     * @brief 监控循环，定时检查文件修改
     */
    void watch_loop();

    /**
     * @brief 获取文件最后修改时间
     * @return 文件修改时间，失败返回 nullopt
     */
    std::optional<std::filesystem::file_time_type> get_file_modification_time() const;

    /**
     * @brief 检查文件是否存在
     * @return 文件是否存在
     */
    bool file_exists() const;

    IoContext& io_context_;
    std::string file_path_;
    std::chrono::milliseconds check_interval_;
    std::vector<Callback> callbacks_;
    std::atomic<bool> running_;
    std::optional<std::filesystem::file_time_type> last_mod_time_;
    Config current_config_;
    std::unique_ptr<Timer> timer_;
};

} // namespace config
END_NAMESPACE_COMMON
