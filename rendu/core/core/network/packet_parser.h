/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_PACKET_PARSER_H
#define RENDU_PACKET_PARSER_H


#include "a_service.h"
#include "base/memory/bit_converter.h"
#include "circular_buffer.h"
#include "common/common_define.h"
#include "exception/out_of_range_exception.h"
#include "packet.h"
#include "serialize/memory_buffer.h"

RD_NAMESPACE_BEGIN


    enum ParserState {
      PacketSize,
      PacketBody
    };

    class PacketParser {
    private :
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

      bool Parse(MemoryBuffer *&memoryBuffer) {
        while (true) {
          switch (m_state) {
            case ParserState::PacketSize: {
              if (m_service->m_serviceType == ServiceType::Inner) {
                if (m_buffer->GetLength() < InnerPacketSizeLength) {
                  memoryBuffer = nullptr;
                  return false;
                }

                m_buffer->Read(m_cache, 0, InnerPacketSizeLength);

                m_packetSize = BitConverter::ToInt32(m_cache, 0);
                if (m_packetSize > std::numeric_limits<uint16>::max() * 16 || m_packetSize < Packet::MinPacketSize) {
                  throw Exception("recv packet Size error, 可能是外网探测端口: {}", m_packetSize);
                }
              } else {
                if (m_buffer->GetLength() < OuterPacketSizeLength) {
                  memoryBuffer = nullptr;
                  return false;
                }

                m_buffer->Read(m_cache, 0, OuterPacketSizeLength);

                m_packetSize = BitConverter::ToUInt16(m_cache, 0);
                if (m_packetSize < Packet::MinPacketSize) {
                  throw Exception("recv packet Size error, 可能是外网探测端口: {}", m_packetSize);
                }
              }

              m_state = ParserState::PacketBody;
              break;
            }
            case ParserState::PacketBody: {
              if (m_buffer->GetLength() < m_packetSize) {
                memoryBuffer = nullptr;
                return false;
              }

              memoryBuffer = m_service->Fetch(m_packetSize);
              m_buffer->Read(memoryBuffer, m_packetSize);
              //memoryStream.SetLength(m_packetSize - Packet.MessageIndex);

              if (m_service->GetServiceType() == ServiceType::Inner) {
                memoryBuffer->Seek(Packet::MessageIndex, SeekOrigin::Begin);
              } else {
                memoryBuffer->Seek(Packet::OpcodeLength, SeekOrigin::Begin);
              }

              m_state = ParserState::PacketSize;
              return true;
            }
            default:
              throw OutOfRangeException();
          }
        }
      }
    };

RD_NAMESPACE_END

#endif //RENDU_PACKET_PARSER_H
