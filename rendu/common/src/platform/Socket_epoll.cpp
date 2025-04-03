/*
* Created by boil on 25-4-9.
*/
#ifdef __linux__

#include "common/platform/Socket.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace platform {

Socket::Socket() : fd_(-1) {}

Socket::~Socket() { close(); }

void Socket::createTcp() {
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        throw std::system_error(errno, std::system_category(), "socket() failed");
    }
}

void Socket::bind(const std::string& ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        throw std::system_error(errno, std::system_category(), "inet_pton failed");
    }

    if (::bind(fd_, (s*)&addr, sizeof(addr)) < 0) {
        throw std::system_error(errno, std::system_category(), "bind failed");
    }
}

void Socket::listen(int backlog) {
    if (::listen(fd_, backlog) < 0) {
        throw std::system_error(errno, std::system_category(), "listen failed");
    }
}

std::unique_ptr<Socket> Socket::accept() {
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = ::accept(fd_, (sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        throw std::system_error(errno, std::system_category(), "accept failed");
    }

    auto client = std::make_unique<Socket>();
    client->fd_ = client_fd;
    return client;
}

void Socket::connect(const std::string& ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        throw std::system_error(errno, std::system_category(), "inet_pton failed");
    }

    if (::connect(fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::system_error(errno, std::system_category(), "connect failed");
    }
}

ssize_t Socket::read(void* buf, size_t len) {
    ssize_t n = ::read(fd_, buf, len);
    if (n < 0) {
        throw std::system_error(errno, std::system_category(), "read failed");
    }
    return n;
}

ssize_t Socket::write(const void* buf, size_t len) {
    ssize_t n = ::write(fd_, buf, len);
    if (n < 0) {
        throw std::system_error(errno, std::system_category(), "write failed");
    }
    return n;
}

void Socket::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

void Socket::setNonBlocking(bool enable) {
    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags == -1) {
        throw std::system_error(errno, std::system_category(), "fcntl get failed");
    }

    if (enable) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(fd_, F_SETFL, flags) == -1) {
        throw std::system_error(errno, std::system_category(), "fcntl set failed");
    }
}

} // namespace platform

#endif