// LogSystem.hpp
#pragma once



#include "LogComponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"
#include "common/logger/LogAppender.hpp"

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <algorithm>

class LogSystem : public core::System {
public:
    explicit LogSystem(core::World& world)
        : System(world)
    {
        addAppender<logger::ConsoleAppender>();
        workerThread_ = std::thread(&LogSystem::processLogs, this);
    }

    template<typename T, typename... Args>
    void addAppender(Args&&... args) {
        auto appender = std::make_shared<T>(std::forward<Args>(args)...);
        std::lock_guard<std::mutex> lock(appendersMutex_);
        appenders_.push_back(appender);
    }

    void log(logger::LogLevel level, const std::string& category,
             const std::string& message) {
        auto& entity = world_.createEntity();
        entity.addComponent<LogComponent>(level, category, message);
    }

    ~LogSystem() {
        running_ = false;
        if(workerThread_.joinable()) {
            workerThread_.join();
        }
    }

    void update(double) override {
        auto entities = world_.getEntitiesWith<LogComponent>();
        std::lock_guard<std::mutex> lock(queueMutex_);

        for(auto* entity : entities) {
            if(auto log = entity->getComponent<LogComponent>()) {
                if(shouldProcess(*log)) {
                    logQueue_.push(*log);
                    entity->removeComponent<LogComponent>();
                }
            }
        }
    }

private:
    std::vector<std::shared_ptr<logger::LogAppender>> appenders_;
    std::queue<LogComponent> logQueue_;
    std::mutex queueMutex_;
    std::mutex appendersMutex_;
    std::atomic<bool> running_{true};
    std::thread workerThread_;

    void processLogs() {
        while(running_ || !logQueue_.empty()) {
            std::vector<LogComponent> batch;
            {
                std::lock_guard<std::mutex> lock(queueMutex_);
                for(size_t i = 0; i < 100 && !logQueue_.empty(); ++i) {
                    batch.push_back(std::move(logQueue_.front()));
                    logQueue_.pop();
                }
            }

            for(auto& log : batch) {
                formatAndWrite(log);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void formatAndWrite(const LogComponent& log) {
        const auto t = std::chrono::system_clock::to_time_t(log.timestamp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&t), "%F %T")
            << " [" << logLevelToString(log.level) << "] "
            << "[" << log.category << "] "
            << log.message;

        const std::string formatted = oss.str();

        std::lock_guard lock(appendersMutex_);
        for(auto& appender : appenders_) {
            if(appender->shouldLog(log.level)) {
                appender->write(formatted);
            }
        }
    }

    bool shouldProcess(const LogComponent& log)  {
        std::lock_guard<std::mutex> lock(appendersMutex_);
        return std::any_of(appenders_.begin(), appenders_.end(),
            [&log](auto& a) { return a->shouldLog(log.level); });
    }

    static const char* logLevelToString(logger::LogLevel level) {
        switch(level) {
            case logger::LogLevel::TRACE: return "TRACE";
            case logger::LogLevel::DEBUG: return "DEBUG";
            case logger::LogLevel::INFO: return "INFO";
            case logger::LogLevel::WARN: return "WARN";
            case logger::LogLevel::ERROR: return "ERROR";
            case logger::LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
};