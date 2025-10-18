// platform/Socket.hpp
#pragma once

#include <string>
#include <system_error>
#include <memory>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace platform
{
    class Socket
    {
    public:
        Socket();
        ~Socket();

        static std::unique_ptr<Socket> create() {
            return std::make_unique<Socket>();
        }
        // 基础操作
        void createTcp();
        void bind(const std::string& ip, uint16_t port);
        void listen(int backlog = 128);
        std::unique_ptr<Socket> accept();
        void connect(const std::string& ip, uint16_t port);
        ssize_t read(void* buf, size_t len);
        ssize_t write(const void* buf, size_t len);
        void close();
        void setNonBlocking(bool enable);

        // 平台相关句柄
#ifdef _WIN32
        using Handle = SOCKET;
#else
        using Handle = int;
#endif
        const Handle invalid_handle = -1;

        Handle nativeHandle() const { return fd_; }


    private:
        Handle fd_;



    };
} // namespace platform
