/*
* Created by boil on 2023/10/27.
*/

#include "buffer_call_back.h"

RD_NAMESPACE_BEGIN

  BufferCallBack::~BufferCallBack() {
    SendCompleted(false);
  }

  BufferCallBack::BufferCallBack(List<std::pair<ABuffer::Ptr, bool>> list,
                                        BufferList::SendResult cb)
    : _cb(std::move(cb)), _pkt_list(std::move(list)) {}

  void BufferCallBack::SendCompleted(bool flag) {
    if (_cb) {
      //全部发送成功或失败回调
      while (!_pkt_list.empty()) {
        _cb(_pkt_list.front().first, flag);
        _pkt_list.pop_front();
      }
    } else {
      _pkt_list.clear();
    }
  }

  void BufferCallBack::SendFrontSuccess() {
    if (_cb) {
      //发送成功回调
      _cb(_pkt_list.front().first, true);
    }
    _pkt_list.pop_front();
  }

RD_NAMESPACE_END
