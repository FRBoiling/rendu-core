// src/IOHandler_epoll.cpp
#ifdef __linux__

#include "common/platform/IOHandler.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <system_error>

namespace platform {

class EpollHandler : public IOHandler {
public:
    EpollHandler() {
        epollFd_ = epoll_create1(0);
        if (epollFd_ == -1) {
            throw std::system_error(errno, std::system_category(), "epoll_create1 failed");
        }
    }

    ~EpollHandler() {
        close(epollFd_);
    }

    void addSocket(Socket::Handle fd, unsigned events) override {
        struct epoll_event ev;
        ev.events = translateEvents(events);
        ev.data.fd = fd;

        if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
            throw std::(errno, std::system_category(), "epoll_ctl add failed");
        }
    }

    void modifySocket(Socket::Handle fd, unsigned events) override {
        struct epoll_event ev;
        ev.events = translateEvents(events);
        ev.data.fd = fd;

        if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
            throw std::system_error(errno, std::system_category(), "epoll_ctl mod failed");
        }
    }

    void removeSocket(Socket::Handle fd) override {
        if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
            throw std::system_error(errno, std::system_category(), "epoll_ctl del failed");
        }
    }

    void poll(int timeoutMs) override {
        constexpr int MAX_EVENTS = 64;
        struct epoll_event events[MAX_EVENTS];

        int n = epoll_wait(epollFd_, events, MAX_EVENTS, timeoutMs);
        if (n == -1) {
            if (errno != EINTR) {
                throw std::system_error(errno, std::system_category(), "epoll_wait failed");
            }
            return;
        }

        for (int i = 0; i < n; ++i) {
            unsigned revents = 0;
            if (events[i].events & EPOLLIN)  revents |= Read;
            if (events[i].events & EPOLLOUT) revents |= Write;
            if (events[i].events & EPOLLERR) revents |= Error;

            if (callback_ && revents != 0) {
                callback_(events[i].data.fd, revents);
            }
        }
    }

private:
    int epollFd_;

    static int translateEvents(unsigned events) {
        int e = 0;
        if (events & Read)  e |= EPOLLIN;
        if (events & Write) e |= EPOLLOUT;
        if (events & Error) e |= EPOLLERR;
        return e | EPOLLET; // 边缘触发模式
    }
};

std::unique_ptr<IOHandler> IOHandler::create() {
    return std::make_unique<EpollHandler>();
}

} // namespace platform
#endif