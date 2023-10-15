/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_BUFFER_CALL_BACK_H
#define RENDU_BUFFER_CALL_BACK_H

#include "buffer_list.h"
#include "utils/list_exp.h"

RD_NAMESPACE_BEGIN

  class BufferCallBack {
  public:
    BufferCallBack(List<std::pair<ABuffer::Ptr, bool> > list, BufferList::SendResult cb);

    ~BufferCallBack();

  public:
    void SendCompleted(bool flag);

    void SendFrontSuccess();

  protected:
    BufferList::SendResult _cb;
    List<std::pair<ABuffer::Ptr, bool> > _pkt_list;
  };

RD_NAMESPACE_END

#endif //RENDU_BUFFER_CALL_BACK_H
