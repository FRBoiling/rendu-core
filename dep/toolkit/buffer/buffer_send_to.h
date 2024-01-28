/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_BUFFER_SEND_TO_H
#define RENDU_BUFFER_SEND_TO_H

#include "buffer_list.h"
#include "utils/list_exp.h"
#include "buffer_call_back.h"

RD_NAMESPACE_BEGIN

  class BufferSendTo final : public BufferList, public BufferCallBack {
  public:
    BufferSendTo(List<std::pair<ABuffer::Ptr, bool> > list, SendResult cb, bool is_udp);

    ~BufferSendTo() override = default;

    bool IsEmpty() override;

    size_t Count() override;

    ssize_t Send(int fd, int flags) override;

  private:
    bool _is_udp;
    size_t _offset = 0;
  };

RD_NAMESPACE_END

#endif //RENDU_BUFFER_SEND_TO_H
