/*
* Created by boil on 25-4-9.
*/
#ifdef _WIN32
#include "platform/Socket.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdexcept>

#pragma comment(lib, "Ws2_32.lib")

namespace platform {

struct WSAInit {
    WSAInit() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }
    ~WSAInit() { WSACleanup(); }
} wsaInit;

Socket::Socket() : fd_(INVALID_SOCKET) {}

Socket::~Socket() { close(); }

void Socket::createTcp() {
    fd_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_ == INVALID_SOCKET) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "socket() failed");
    }
}

void Socket::bind(const std::string& ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (InetPtonA(AF_INET, ip.c_str(), &addr.sin_addr) != 1) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "InetPton failed");
    }

    if (::bind(fd_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::system_error(etLastError(), std::system_category(), "bind failed");
    }
}

void Socket::listen(int backlog) {
    if (::listen(fd_, backlog) == SOCKET_ERROR) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "listen failed");
    }
}

std::unique_ptr<Socket> Socket::accept() {
    SOCKET client_fd = ::accept(fd_, nullptr, nullptr);
    if (client_fd == INVALID_SOCKET) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "accept failed");
    }

    auto client = std::make_unique<Socket>();
    client->fd_ = client_fd;
    return client;
}

void Socket::connect(const std::string& ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (InetPtonA(AF_INET, ip.c_str(), &addr.sin_addr) != 1) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "InetPton failed");
    }

    if (::connect(fd_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "connect failed");
    }
}

ssize_t Socket::read(void* buf, size_t len) {
    int n = ::recv(fd_, static_cast<char*>(buf), len, 0);
    if (n == SOCKET_ERROR) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "recv failed");
    }
    return n;
}

ssize_t Socket::write(const void* buf, size_t len) {
    int n = ::send(fd_, static_cast<const char*>(buf), len, 0);
    if (n == SOCKET_ERROR) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "send failed");
    }
    return n;
}

void Socket::close() {
    if (fd_ != INVALID_SOCKET) {
        ::closesocket(fd_);
        fd_ = INVALID_SOCKET;
    }
}

void Socket::setNonBlocking(bool enable) {
    u_long mode = enable ? 1 : 0;
    if (ioctlsocket(fd_, FIONBIO, &mode) == SOCKET_ERROR) {
        throw std::system_error(WSAGetLastError(), std::system_category(), "ioctlsocket failed");
    }
}

} // namespace platform
#endif

