/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_PACKET_PARSER_H
#define RENDU_PACKET_PARSER_H

#include "core_define.h"

#include "a_service.h"
#include "base/memory/bit_converter.h"
#include "circular_buffer.h"
#include "exception/out_of_range_exception.h"
#include "packet.h"
#include "serialize/memory_buffer.h"

CORE_NAMESPACE_BEGIN

enum ParserState {
  PacketSize,
  PacketBody
};

class PacketParser {
private:
  CircularBuffer *m_buffer;
  AService *m_service;

  int m_packetSize;
  ParserState m_state;
  byte m_cache[8]{};
  const int InnerPacketSizeLength = 4;
  const int OuterPacketSizeLength = 2;
  MemoryBuffer *m_memoryBuffer;

public:
  PacketParser(CircularBuffer *buffer, AService *service)
      : m_buffer(buffer), m_service(service), m_packetSize(0), m_state(ParserState::PacketSize),
        m_memoryBuffer(nullptr) {
  }

  bool Parse(MemoryBuffer *&memoryBuffer);
};

CORE_NAMESPACE_END

#endif//RENDU_PACKET_PARSER_H
