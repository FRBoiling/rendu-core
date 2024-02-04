/*
* Created by boil on 2023/10/15.
*/

#include "a_channel.h"

CORE_NAMESPACE_BEGIN
core::AChannel::AChannel() {}

AChannel::AChannel(ChannelType channel_type, IPEndPoint remote_address, INT64 id)
    : m_channel_type(channel_type), m_remote_address(remote_address), m_id(id) {
}

AChannel::~AChannel() {}


CORE_NAMESPACE_END