/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_PACKET_H
#define RENDU_PACKET_H

#include "core_define.h"
#include "serialize/memory_buffer.h"

RD_NAMESPACE_BEGIN

    struct Packet {
    public:
      static const int MinPacketSize = 2;
      static const int OpcodeIndex = 16;
      static const int KcpOpcodeIndex = 0;
      static const int OpcodeLength = 2;
      static const int ActorIdIndex = 0;
      static const int ActorIdLength = 16;
      static const int MessageIndex = 18;

      UINT16 Opcode;
      INT64 ActorId;
      MemoryStream MemoryStream;
    };

RD_NAMESPACE_END

#endif //RENDU_PACKET_H
