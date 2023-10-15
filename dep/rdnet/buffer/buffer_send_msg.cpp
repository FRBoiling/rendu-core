/*
* Created by boil on 2023/10/27.
*/

#include "buffer_send_msg.h"
#include "utils/object_statistic.h"
#include "poller/errno/errno.h"

RD_NAMESPACE_BEGIN

  bool BufferSendMsg::IsEmpty() {
    return _remain_size == 0;
  }

  size_t BufferSendMsg::Count() {
    return _iovec.size() - _iovec_off;
  }

  ssize_t BufferSendMsg::send_l(int fd, int flags) {
    ssize_t n;
#if !defined(_WIN32)
    do {
      struct msghdr msg;
      msg.msg_name = nullptr;
      msg.msg_namelen = 0;
      msg.msg_iov = &(_iovec[_iovec_off]);
      msg.msg_iovlen = _iovec.size() - _iovec_off;
      if (msg.msg_iovlen > IOV_MAX) {
        msg.msg_iovlen = IOV_MAX;
      }
      msg.msg_control = nullptr;
      msg.msg_controllen = 0;
      msg.msg_flags = flags;
      n = sendmsg(fd, &msg, flags);
    } while (-1 == n && RD_EINTR == get_rd_error(true));
#else
    do {
        DWORD sent = 0;
        n = WSASend(fd, const_cast<LPWSABUF>(&_iovec[0]), static_cast<DWORD>(_iovec.size()), &sent, static_cast<DWORD>(flags), 0, 0);
        if (n == SOCKET_ERROR) {return -1;}
        n = sent;
    } while (n < 0 && RD_ECANCELED == get_uv_error(true));
#endif

    if (n >= (ssize_t)_remain_size) {
      //全部写完了
      _remain_size = 0;
      SendCompleted(true);
      return n;
    }

    if (n > 0) {
      //部分发送成功
      reOffset(n);
      return n;
    }

    //一个字节都未发送
    return n;
  }

  ssize_t BufferSendMsg::Send(int fd, int flags) {
    auto remain_size = _remain_size;
    while (_remain_size && send_l(fd, flags) != -1);

    ssize_t sent = remain_size - _remain_size;
    if (sent > 0) {
      //部分或全部发送成功
      return sent;
    }
    //一个字节都未发送成功
    return -1;
  }

  void BufferSendMsg::reOffset(size_t n) {
    _remain_size -= n;
    size_t offset = 0;
    for (auto i = _iovec_off; i != _iovec.size(); ++i) {
      auto &ref = _iovec[i];
#if !defined(_WIN32)
      offset += ref.iov_len;
#else
      offset += ref.len;
#endif
      if (offset < n) {
        //此包发送完毕
        SendFrontSuccess();
        continue;
      }
      _iovec_off = i;
      if (offset == n) {
        //这是末尾发送完毕的一个包
        ++_iovec_off;
        SendFrontSuccess();
        break;
      }
      //这是末尾发送部分成功的一个包
      size_t remain = offset - n;
#if !defined(_WIN32)
      ref.iov_base = (char *)ref.iov_base + ref.iov_len - remain;
      ref.iov_len = remain;
#else
      ref.buf = (CHAR *)ref.buf + ref.len - remain;
        ref.len = remain;
#endif
      break;
    }
  }

  BufferSendMsg::BufferSendMsg(List<std::pair<ABuffer::Ptr, bool>> list, SendResult cb)
    : BufferCallBack(std::move(list), std::move(cb))
    , _iovec(_pkt_list.size()) {
    auto it = _iovec.begin();
    _pkt_list.For_Each([&](std::pair<ABuffer::Ptr, bool> &pr) {
#if !defined(_WIN32)
      it->iov_base = pr.first->Data();
      it->iov_len = pr.first->Size();
      _remain_size += it->iov_len;
#else
      it->buf = pr.first->data();
        it->len = pr.first->size();
        _remain_size += it->len;
#endif
      ++it;
    });
  }


RD_NAMESPACE_END