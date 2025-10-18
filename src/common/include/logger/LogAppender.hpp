// LogAppender.hpp
#pragma once

#include "LogLevel.hpp"

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <fstream>
#include <iostream>

namespace logger{

// 抽象日志输出接口
class LogAppender {
public:
    virtual ~LogAppender() = default;
    virtual void write(const std::string& message) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual bool shouldLog(LogLevel level) const = 0;
};

// 控制台输出
class ConsoleAppender : public LogAppender {
    std::mutex mutex_;
    LogLevel level_ = LogLevel::DEBUG;

public:
    void write(const std::string& msg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << msg << std::endl;
    }

    void setLevel(LogLevel level) override { level_ = level; }

    bool shouldLog(LogLevel level) const override {
        return level >= level_;
    }
};

// 文件输出
class FileAppender : public LogAppender {
    std::ofstream file_;
    std::mutex mutex_;
    LogLevel level_ = LogLevel::INFO;

public:
    explicit FileAppender(const std::string& filename)
        : file_(filename, std::ios::app) {}

    ~FileAppender() override {
        if(file_.is_open()) {
            file_.close();
        }
    }

    void write(const std::string& msg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if(file_.is_open()) {
            file_ << msg << "\n";
            file_.flush();
        }
    }

    void setLevel(LogLevel level) override { level_ = level; }

    bool shouldLog(LogLevel level) const override {
        return level >= level_;
    }
};

// 网络输出（示例）
class NetworkAppender : public LogAppender {
    // 实现网络传输逻辑...
};

}