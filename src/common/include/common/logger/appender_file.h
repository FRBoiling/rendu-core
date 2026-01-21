//
// Created by boil on 2026/1/20.
//
// ============================================================================
// appender_file.h - 文件日志追加器（支持同步/异步模式）
// ============================================================================

#ifndef RENDU_APPENDER_FILE_H
#define RENDU_APPENDER_FILE_H

#include "appender.h"
#include "common/asio/io_context.h"
#include <fstream>
#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <deque>
#include <atomic>
#include <condition_variable>

BEGIN_NAMESPACE_COMMON

// 前置声明
struct LogMessage;

// ============================================================================
// AppenderFile - 文件日志追加器
// 支持同步和异步两种模式
// ============================================================================

class RC_COMMON_API AppenderFile : public Appender
{
public:
    static constexpr AppenderType type = AppenderType::APPENDER_FILE;

    /**
     * @brief 构造函数
     * @param id 追加器 ID
     * @param name 追加器名称
     * @param level 最低日志级别
     * @param flags 追加器标志（可设置 APPENDER_FLAGS_ASYNC 启用异步模式）
     * @param extraArgs 额外参数
     *   - [0]: 文件路径
     *   - [1]: 批量大小（异步模式有效，默认 100）
     *   - [2]: 刷新间隔 ms（异步模式有效，默认 1000）
     *   - [3]: 最大文件大小字节（轮转，默认 100MB）
     */
    AppenderFile(
        uint8 id,
        std::string name,
        LogLevel level,
        AppenderFlags flags,
        std::vector<std::string_view> const& extraArgs
    );

    /**
     * @brief 析构函数
     */
    ~AppenderFile() override;

    /**
     * @brief 获取追加器类型
     */
    [[nodiscard]] AppenderType getType() const override { return type; }

    /**
     * @brief 设置 IoContext（异步模式必须调用）
     * @param ioContext IoContext 指针
     */
    void setIoContext(Asio::IoContext* ioContext);

    /**
     * @brief 设置批量大小（异步模式）
     * @param batchSize 批量大小
     */
    void setBatchSize(size_t batchSize);

    /**
     * @brief 设置刷新间隔（异步模式）
     * @param intervalMs 刷新间隔（毫秒）
     */
    void setFlushInterval(uint32 intervalMs);

    /**
     * @brief 设置文件轮转配置
     * @param maxSize 最大文件大小（字节）
     * @param maxBackups 最大备份数量
     */
    void setFileRotationConfig(uint64 maxSize, uint32 maxBackups);

    /**
     * @brief 强制刷新缓冲区
     */
    void flush();

    /**
     * @brief 获取待处理消息数量（异步模式）
     */
    [[nodiscard]] size_t getPendingCount() const;

private:
    /**
     * @brief 实际写入日志消息
     */
    void _write(LogMessage const* message) override;

    /**
     * @brief 同步写入
     */
    void _writeSync(LogMessage const* message);

    /**
     * @brief 异步写入（提交到队列）
     */
    void _writeAsync(LogMessage const* message);

    /**
     * @brief 异步批量刷新处理
     */
    void _asyncFlush();

    /**
     * @brief 打开日志文件
     */
    bool _openFile();

    /**
     * @brief 关闭日志文件
     */
    void _closeFile();

    /**
     * @brief 检查并执行文件轮转
     */
    void _checkRotation();

    /**
     * @brief 执行文件轮转
     */
    void _rotateFile();

    // 成员变量
    std::ofstream m_file;                           ///< 文件流
    std::string m_filename;                         ///< 文件名
    Asio::IoContext* m_ioContext = nullptr;         ///< IoContext（异步模式）
    std::unique_ptr<Asio::ExecutorWorkGuard> m_workGuard; ///< 工作守卫（异步模式）

    // 异步模式相关
    bool m_async = false;                           ///< 是否异步模式
    std::deque<std::string> m_messageQueue;         ///< 消息队列
    mutable std::mutex m_queueMutex;                ///< 队列互斥锁
    size_t m_batchSize = 100;                       ///< 批量大小
    size_t m_pendingCount = 0;                      ///< 待处理计数
    std::atomic<bool> m_flushing{false};            ///< 正在刷新

    // 文件轮转相关
    uint64 m_maxFileSize = 100 * 1024 * 1024;      ///< 最大文件大小（100MB）
    uint32 m_maxBackups = 10;                       ///< 最大备份数量
    uint64 m_currentSize = 0;                       ///< 当前文件大小
};

END_NAMESPACE_COMMON

#endif // RENDU_APPENDER_FILE_H
