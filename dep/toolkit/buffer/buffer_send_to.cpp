/*
* Created by boil on 2023/10/27.
*/

#include "buffer_send_to.h"
#include "net/socket/buffer_socket.h"
#include "utils/rd_errno.h"

RD_NAMESPACE_BEGIN

  BufferSendTo::BufferSendTo(List<std::pair<ABuffer::Ptr, bool>> list, BufferList::SendResult cb, bool is_udp)
    : BufferCallBack(std::move(list), std::move(cb)), _is_udp(is_udp) {}

  bool BufferSendTo::IsEmpty() {
    return _pkt_list.empty();
  }

  size_t BufferSendTo::Count() {
    return _pkt_list.size();
  }

  static inline BufferSocket *getBufferSockPtr(std::pair<ABuffer::Ptr, bool> &pr) {
    if (!pr.second) {
      return nullptr;
    }
    return static_cast<BufferSocket *>(pr.first.get());
  }

  ssize_t BufferSendTo::Send(int fd, int flags) {
    size_t sent = 0;
    ssize_t n;
    while (!_pkt_list.empty()) {
      auto &front = _pkt_list.front();
      auto &buffer = front.first;
      if (_is_udp) {
        auto ptr = getBufferSockPtr(front);
        n = ::sendto(fd, buffer->Data() + _offset, buffer->Size() - _offset, flags, ptr ? ptr->GetSockAddr() : nullptr,
                     ptr ? ptr->GetSockLen() : 0);
      } else {
        n = ::send(fd, buffer->Data() + _offset, buffer->Size() - _offset, flags);
      }

      if (n >= 0) {
        assert(n);
        _offset += n;
        if (_offset == buffer->Size()) {
          SendFrontSuccess();
          _offset = 0;
        }
        sent += n;
        continue;
      }

      //n == -1的情况
      if (GetError(true) == RD_EINTR) {
        //被打断，需要继续发送
        continue;
      }
      //其他原因导致的send返回-1
      break;
    }
    return sent ? sent : -1;
  }

RD_NAMESPACE_END