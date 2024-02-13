/*
* Created by boil on 2024/2/18.
*/

#ifndef RENDU_NET_NET_INTEROP_SAFE_HANDLE_H_
#define RENDU_NET_NET_INTEROP_SAFE_HANDLE_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

namespace interop {

  class SafeHandle : public NonCopyable {
  public:
    using Ptr = std::shared_ptr<SafeHandle>;

  public:
    SafeHandle() : m_fd(-1){};
    SafeHandle(int fd) : m_fd(fd){};
    virtual ~SafeHandle() { ReleaseHandle(); };

    // 允许移动构造函数和移动赋值操作符，以支持转移所有权
    SafeHandle(SafeHandle &&other) : m_fd(other.m_fd) {
      other.m_fd = -1;
    }

    SafeHandle &operator=(SafeHandle &&other) {
      if (this != &other) {
        m_fd = other.m_fd;
        other.m_fd = -1;
      }
      return *this;
    }

  public:
    bool IsInvalid() const { return m_fd == -1; }
    int RawHandle() const { return m_fd; }
    void SetHandleAsInvalid() { Close(); }
    virtual void SetHandle(int fd) { m_fd = fd; }

  protected:
    virtual bool ReleaseHandle() { return Close() != -1; }

  protected:
    int Close();

  private:
    int m_fd;
  };

}// namespace interop

NET_NAMESPACE_END

#endif//RENDU_NET_NET_INTEROP_SAFE_HANDLE_H_
