// src/IOHandler_iocp.cpp
#ifdef _WIN32

#include "common/platform/IOHandler.hpp"

#include <windows.h>
#include <winsock2.h>
#include <system_error>
#include <mutex>
#include <vector>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")

namespace platform {

class IocpHandler : public IOHandler {
public:
    IocpHandler() : iocpHandle_(INVALID_HANDLE_VALUE), running_(true) {
        iocpHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (!iocpHandle_) {
            throw std::system_error(GetLastError(), std::system_category(), "CreateIoCompletionPort failed");
        }
        startWorkerThreads();
    }

    ~IocpHandler() {
        running_ = false;
        PostQueuedCompletionStatus(iocpHandle_, 0, 0, NULL);
        for (auto& thread : workerThreads_) {
            if (thread.joinable()) thread.join();
        }
        CloseHandle(iocpHandle_);
    }

    void addSocket(Socket::Handle fd, unsigned events)        std::lock_guard<std::mutex> lock(mutex_);

        if (CreateIoCompletionPort((HANDLE)fd, iocpHandle_, (ULONG_PTR)fd, 0) == NULL) {
            throw std::system_error(GetLastError(), std::system_category(), "AssociateSocket failed");
        }

        if (events & Read) postRecv(fd);
    }

    void modifySocket(Socket::Handle fd, unsigned events) override {
        // IOCP自动管理无需显式修改
    }

    void removeSocket(Socket::Handle fd) override {
        std::lock_guard<std::mutex> lock(mutex_);
        closesocket(fd);
    }

    void poll(int timeoutMs) override {
        // Windows使用独立工作线程处理IOCP
    }

private:
    HANDLE iocpHandle_;
    std::vector<std::thread> workerThreads_;
    std::mutex mutex_;
    std::atomic<bool> running_;

    enum { OP_READ = 1, OP_WRITE = 2 };

    void startWorkerThreads() {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors * 2; ++i) {
            workerThreads_.emplace_back([this] { workerProc(); });
        }
    }

    void workerProc() {
        while (running_) {
            DWORD bytesTransferred = 0;
            ULONG_PTR completionKey = 0;
            LPOVERLAPPED overlapped = nullptr;

            BOOL result = GetQueuedCompletionStatus(
                iocpHandle_,
                &bytesTransferred,
                &completionKey,
                &overlapped,
                INFINITE
            );

            if (!running_) break;

            if (result == 0) {
                DWORD error = GetLastError();
                if (overlapped == NULL) continue;
                handleError((SOCKET)completionKey, error);
                continue;
            }

            if (callback_) {
                auto opType = *(DWORD*)overlapped;
                unsigned events = 0;
                if (opType == OP_READ) {
                    events |= Read;
                    postRecv((SOCKET)completionKey);
                }
                callback_((SOCKET)completionKey, events);
            }
            delete overlapped;
        }
    }

    void postRecv(SOCKET fd) {
        auto overlapped = new OVERLAPPED{};
        *(DWORD*)overlapped = OP_READ;

        WSABUF buf{};
        DWORD flags = 0;
        if (WSARecv(fd, &buf, 1, NULL, &flags, overlapped, NULL) == SOCKET_ERROR) {
            DWORD error = WSAGetLastError();
            if (error != WSA_IO_PENDING) {
                delete overlapped;
                throw std::system_error(error, std::system_category(), "WSARecv failed");
            }
        }
    }

    void handleError(SOCKET fd, DWORD error) {
        if (callback_) callback_(fd, Error);
        removeSocket(fd);
    }
};

std::unique_ptr<IOHandler> IOHandler::create() {
    return std::make_unique<IocpHandler>();
}

} // namespace platform
#endif