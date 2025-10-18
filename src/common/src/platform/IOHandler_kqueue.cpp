// src/IOHandler_kqueue.cpp
#ifdef __APPLE__

#include "common/platform/IOHandler.hpp"

#include <sys/event.h>
#include <system_error>
#include <unistd.h>

namespace platform {

class KqueueHandler : public IOHandler {
public:
    KqueueHandler() {
        kq_ = kqueue();
        if (kq_ == -1) {
            throw std::system_error(errno, std::system_category(), "kqueue failed");
        }
    }

    ~KqueueHandler() {
        close(kq_);
    }

    void addSocket(Socket::Handle fd, unsigned events) override {
        struct kevent ev[2];
        int n = 0;

        if (events & Read) {
            EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, nullptr);
        }
        if (events & Write) {
            EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, nullptr);
        }

        if (kevent(kq_, ev, n, nullptr, 0, nullptr) == -1) {
            throw std::system_error(errno, std::system_category(), "kevent add failed");
        }
    }

    void modifySocket(Socket::Handle fd, unsigned events) override {
        removeSocket(fd);
        addSocket(fd, events);
    }

    void removeSocket(Socket::Handle fd) override {
        struct kevent ev[2];
        int n = 0;

        EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
        EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

        kevent(kq_, ev, 2, nullptr, 0, nullptr);
    }

    void poll(int timeoutMs) override {
        struct timespec timeout {
            .tv_sec = timeoutMs/ 1000,
            .tv_nsec = (timeoutMs % 1000) * 1000000L
        };

        constexpr int MAX_EVENTS = 64;
        struct kevent events[MAX_EVENTS];

        int n = kevent(kq_, nullptr, 0, events, MAX_EVENTS, &timeout);
        if (n == -1) {
            if (errno != EINTR) {
                throw std::system_error(errno, std::system_category(), "kevent failed");
            }
            return;
        }

        for (int i = 0; i < n; ++i) {
            unsigned revents = 0;
            if (events[i].filter == EVFILT_READ)  revents |= Read;
            if (events[i].filter == EVFILT_WRITE) revents |= Write;
            if (events[i].flags & EV_ERROR)       revents |= Error;

            if (callback_ && revents != 0) {
                callback_(events[i].ident, revents);
            }
        }
    }

private:
    int kq_;
};

std::unique_ptr<IOHandler> IOHandler::create() {
    return std::make_unique<KqueueHandler>();
}

} // namespace platform
#endif