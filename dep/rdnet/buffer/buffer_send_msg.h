/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_BUFFER_SEND_MSG_H
#define RENDU_BUFFER_SEND_MSG_H

#include <sys/socket.h>
#include "buffer_list.h"
#include "buffer_call_back.h"

RD_NAMESPACE_BEGIN

#if defined(_WIN32)
  using SocketBuf = WSABUF;
#else
  using SocketBuf = iovec;
#endif

class BufferSendMsg final : public BufferList, public BufferCallBack {
public:
  using SocketBufVec = std::vector<SocketBuf>;

  BufferSendMsg(List<std::pair<ABuffer::Ptr, bool> > list, SendResult cb);
  ~BufferSendMsg() override = default;

  bool IsEmpty() override;
  size_t Count() override;
  ssize_t Send(int fd, int flags) override;

private:
  void reOffset(size_t n);
  ssize_t send_l(int fd, int flags);

private:
  size_t _iovec_off = 0;
  size_t _remain_size = 0;
  SocketBufVec _iovec;
};

RD_NAMESPACE_END

#endif //RENDU_BUFFER_SEND_MSG_H
