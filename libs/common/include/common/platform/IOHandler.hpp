// include/platform/IOHandler.hpp
#pragma once

#include "Socket.hpp"
#include <functional>
#include <memory>

namespace platform {

    class IOHandler {
    public:
        enum EventType {
            Read  = 0x01,
            Write = 0x02,
            Error = 0x04
        };

        using EventCallback = std::function<void(Socket::Handle, unsigned)>;

        // 工厂方法
        static std::unique_ptr<IOHandler> create();

        virtual ~IOHandler() = default;

        // 核心接口
        virtual void addSocket(Socket::Handle fd, unsigned events) = 0;
        virtual void modifySocket(Socket::Handle fd, unsigned events) = 0;
        virtual void removeSocket(Socket::Handle fd) = 0;
        virtual void poll(int timeoutMs) = 0;

        void setEventCallback(EventCallback cb) { callback_ = std::move(cb); }

    protected:
        EventCallback callback_;
    };

} // namespace platform