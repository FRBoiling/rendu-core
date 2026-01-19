//
// Created by boil on 2026/1/20.
//

#include "common/logger/appender_file.h"
#include "common/logger/log_message.h"
#include "common/asio/io_context.h"
#include "common/asio/executor_work_guard.h"
#include "common/asio/post.h"
#include "common/utils/string_format.h"
#include <filesystem>
#include <chrono>

BEGIN_NAMESPACE_COMMON

AppenderFile::AppenderFile(
    uint8 id,
    std::string name,
    LogLevel level,
    AppenderFlags flags,
    std::vector<std::string_view> const& extraArgs)
    : Appender(id, std::move(name), level, flags)
    , m_async(static_cast<bool>(flags & AppenderFlags::APPENDER_FLAGS_ASYNC))
{
    // 解析文件路径
    if (!extraArgs.empty())
    {
        m_filename = extraArgs[0];
    }
    else
    {
        throw InvalidAppenderArgsException("AppenderFile: Missing filename argument");
    }

    // 解析批量大小
    if (extraArgs.size() > 1)
    {
        auto batchSize = std::stoul(std::string(extraArgs[1]));
        if (batchSize > 0)
        {
            m_batchSize = batchSize;
        }
    }

    // 解析最大文件大小
    if (extraArgs.size() > 2)
    {
        m_maxFileSize = std::stoull(std::string(extraArgs[2]));
    }

    // 打开文件
    _openFile();
}

AppenderFile::~AppenderFile()
{
    flush();
    _closeFile();
}

void AppenderFile::setIoContext(Asio::IoContext* ioContext)
{
    if (!m_async)
    {
        return; // 同步模式不需要 IoContext
    }

    if (!ioContext)
    {
        throw std::invalid_argument("IoContext cannot be null for async mode");
    }

    m_ioContext = ioContext;

    // 创建工作守卫，防止 ioContext 空闲退出
    m_workGuard = std::make_unique<Asio::ExecutorWorkGuard>(*m_ioContext);
}

void AppenderFile::setBatchSize(size_t batchSize)
{
    m_batchSize = batchSize > 0 ? batchSize : 100;
}

void AppenderFile::setFlushInterval(uint32 intervalMs)
{
    // 可以在这里添加定时器刷新机制
    // 暂时简化为基于批量的刷新
}

void AppenderFile::setFileRotationConfig(uint64 maxSize, uint32 maxBackups)
{
    m_maxFileSize = maxSize > 0 ? maxSize : 100 * 1024 * 1024;
    m_maxBackups = maxBackups > 0 ? maxBackups : 10;
}

void AppenderFile::flush()
{
    if (m_async)
    {
        // 异步模式：刷新所有待处理消息
        std::lock_guard<std::mutex> lock(m_queueMutex);

        while (!m_messageQueue.empty())
        {
            const std::string& msg = m_messageQueue.front();

            if (m_file.is_open())
            {
                m_file << msg << std::endl;
                m_currentSize += msg.size() + 1;
                _checkRotation();
            }

            m_messageQueue.pop_front();
        }

        m_pendingCount = 0;

        if (m_file.is_open())
        {
            m_file.flush();
        }
    }
    else
    {
        // 同步模式：直接刷新文件
        if (m_file.is_open())
        {
            m_file.flush();
        }
    }
}

size_t AppenderFile::getPendingCount() const
{
    return m_pendingCount;
}

void AppenderFile::_write(LogMessage const* message)
{
    if (m_async)
    {
        _writeAsync(message);
    }
    else
    {
        _writeSync(message);
    }
}

void AppenderFile::_writeSync(LogMessage const* message)
{
    if (m_file.is_open())
    {
        std::string logLine = message->m_prefix + message->m_text;
        m_file << logLine << std::endl;
        m_currentSize += logLine.size() + 1;
        _checkRotation();
    }
}

void AppenderFile::_writeAsync(LogMessage const* message)
{
    if (!m_ioContext)
    {
        throw std::runtime_error("IoContext not set for async mode");
    }

    // 将消息加入队列
    std::string logLine = message->m_prefix + message->m_text;

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_messageQueue.push_back(std::move(logLine));
        ++m_pendingCount;
    }

    // 检查是否需要批量刷新
    if (m_pendingCount >= m_batchSize && !m_flushing.exchange(true))
    {
        // 提交异步刷新任务
        Asio::Post(*m_ioContext, [this]()
        {
            _asyncFlush();
            m_flushing = false;
        });
    }
}

void AppenderFile::_asyncFlush()
{
    std::lock_guard<std::mutex> lock(m_queueMutex);

    if (!m_file.is_open())
    {
        m_messageQueue.clear();
        m_pendingCount = 0;
        return;
    }

    // 批量写入队列中的消息
    while (!m_messageQueue.empty())
    {
        const std::string& msg = m_messageQueue.front();

        m_file << msg << std::endl;
        m_currentSize += msg.size() + 1;

        m_messageQueue.pop_front();
    }

    m_pendingCount = 0;

    // 检查文件轮转
    _checkRotation();

    // 刷新文件
    m_file.flush();
}

bool AppenderFile::_openFile()
{
    if (m_filename.empty())
    {
        return false;
    }

    // 确保目录存在
    std::filesystem::path filePath(m_filename);
    std::filesystem::path dirPath = filePath.parent_path();

    if (!dirPath.empty() && !std::filesystem::exists(dirPath))
    {
        std::error_code ec;
        std::filesystem::create_directories(dirPath, ec);
        if (ec)
        {
            return false;
        }
    }

    // 打开文件（追加模式）
    m_file.open(m_filename, std::ios::out | std::ios::app);

    if (!m_file.is_open())
    {
        return false;
    }

    // 获取当前文件大小
    m_file.seekp(0, std::ios::end);
    m_currentSize = m_file.tellp();

    return true;
}

void AppenderFile::_closeFile()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

void AppenderFile::_checkRotation()
{
    if (m_currentSize >= m_maxFileSize)
    {
        _rotateFile();
    }
}

void AppenderFile::_rotateFile()
{
    // 关闭当前文件
    _closeFile();

    // 重命名备份文件
    for (int i = static_cast<int>(m_maxBackups) - 1; i >= 0; --i)
    {
        std::string oldFile;
        std::string newFile;

        if (i == 0)
        {
            oldFile = m_filename;
        }
        else
        {
            oldFile = m_filename + "." + std::to_string(i);
        }

        newFile = m_filename + "." + std::to_string(i + 1);

        std::error_code ec;
        if (std::filesystem::exists(oldFile, ec))
        {
            std::filesystem::rename(oldFile, newFile, ec);
        }
    }

    // 删除最旧的备份
    std::string oldestBackup = m_filename + "." + std::to_string(m_maxBackups + 1);
    std::error_code ec;
    std::filesystem::remove(oldestBackup, ec);

    // 重新打开文件
    _openFile();
}

END_NAMESPACE_COMMON
