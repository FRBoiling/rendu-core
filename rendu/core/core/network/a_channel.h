/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_A_CHANNEL_H
#define RENDU_A_CHANNEL_H

#include "core_define.h"
#include "network/socket.h"

RD_NAMESPACE_BEGIN

    enum class ChannelType {
      Connect,
      Accept,
    };

    class AChannel {
    public:
      AChannel() : m_id(0), m_type(ChannelType::Connect), m_error() {};

      virtual ~AChannel() = default;

    public:
      [[nodiscard]] int64 GetId() const {
        return m_id;
      }

      inline void SetError(int error) {
        m_error = error;
      }

      [[nodiscard]] inline bool IsDisposed() const {
        return m_id == 0;
      }
    protected:
      int64 m_id;
      ChannelType m_type;
      int32 m_error;
    };

RD_NAMESPACE_END

#endif //RENDU_A_CHANNEL_H
