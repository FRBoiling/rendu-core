/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_A_CHANNEL_H
#define RENDU_A_CHANNEL_H

#include "core_define.h"
#include "packet.h"

CORE_NAMESPACE_BEGIN

enum class ChannelType {
  Connect,
  Accept,
};


class AChannel {
public:
  AChannel();
  AChannel(ChannelType channel_type,IPEndPoint remote_address,INT64 id);
  virtual ~AChannel();;
protected:
  ChannelType m_channel_type;
  IPEndPoint m_remote_address;
  INT64 m_id;
  INT32 m_error;
public:
  INT64 GetId(){
      return m_id;
  }

  IPEndPoint GetRemoteAddress() {
    return m_remote_address;
  }

  void SetRemoteAddress(IPEndPoint value) {
    m_remote_address = value;
  }

  INT32 SetError(INT32 error){
    m_error = error;
    return m_error;
  }

};

CORE_NAMESPACE_END

#endif//RENDU_A_CHANNEL_H
