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
  AChannel() {}

  AChannel(ChannelType channel_type, IPEndPoint remote_address, INT64 id)
      : m_channel_type(channel_type), m_remote_address(remote_address), m_id(id) {
  }

  virtual ~AChannel() {}

protected:
  INT64 m_id;
  INT32 m_error;

  ChannelType m_channel_type;
  IPEndPoint m_remote_address;

public:
  INT64 GetId() const { return m_id; }
  INT64 SetId(INT64 id) {
    m_id = id;
    return m_id;
  }

  ChannelType GetChannelType() const { return m_channel_type; }
  ChannelType SetChannelType(ChannelType type) {
    m_channel_type = type;
    return m_channel_type;
  }

  IPEndPoint GetRemoteAddress() const { return m_remote_address; }
  IPEndPoint SetRemoteAddress(IPEndPoint value) {
    m_remote_address = value;
    return m_remote_address;
  }

  INT32 GetError() const { return m_error; }
  INT32 SetError(INT32 error) {
    m_error = error;
    return m_error;
  }
};

CORE_NAMESPACE_END

#endif//RENDU_A_CHANNEL_H
