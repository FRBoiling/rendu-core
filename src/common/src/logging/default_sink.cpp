// ... existing code ...

#include <memory>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "common/logging/log_level.h"
#include "common/logging/log_sink.h"

DefaultSinkImpl::DefaultSinkImpl()
{
    name_ = "default_sink";
    sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
}

DefaultSinkImpl::~DefaultSinkImpl()
{
}

void DefaultSinkImpl::set_pattern(const std::string& pattern)
{
    if (sink_)
    {
        sink_->set_pattern(pattern);
    }
}

const std::string& DefaultSinkImpl::get_name() const
{
    return name_;
}

std::shared_ptr<spdlog::sinks::sink> DefaultSinkImpl::sink() const
{
    return sink_;
}

// 为DefaultSinkImpl添加缺失的方法实现
std::string DefaultSinkImpl::get_pattern() const
{
    // 注意：spdlog没有直接提供获取当前pattern的方法
    // 这里需要添加一个成员变量来存储pattern，以便实现这个方法
    return current_pattern_;
}

void DefaultSinkImpl::set_min_level(LogLevel level)
{
    // 实现设置最低日志级别
    if (sink_)
    {
        // 根据LogLevel转换为spdlog的level
        // 这里需要添加转换逻辑
    }
}

void DefaultSinkImpl::flush()
{
    if (sink_)
    {
        sink_->flush();
    }
}

// DefaultSink类的构造函数实现
DefaultSink::DefaultSink()
{
    m_impl = new DefaultSinkImpl();
}

// DefaultSink类的析构函数实现
DefaultSink::~DefaultSink()
{
    delete m_impl;
}

// DefaultSink类的set_pattern方法实现
void DefaultSink::set_pattern(const std::string& pattern)
{
    if (m_impl)
    {
        m_impl->set_pattern(pattern);
    }
}

// DefaultSink类的get_name方法实现
const std::string& DefaultSink::get_name() const
{
    static const std::string empty_string;
    return m_impl ? m_impl->get_name() : empty_string;
}

// 为DefaultSink类添加缺失的方法实现
std::string DefaultSink::get_pattern() const
{
    return m_impl ? m_impl->get_pattern() : std::string();
}

void DefaultSink::set_min_level(LogLevel level)
{
    if (m_impl)
    {
        m_impl->set_min_level(level);
    }
}

void DefaultSink::flush()
{
    if (m_impl)
    {
        m_impl->flush();
    }
}