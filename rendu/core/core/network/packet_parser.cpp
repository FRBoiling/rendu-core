/*
* Created by boil on 2023/10/16.
*/

#include "packet_parser.h"

CORE_NAMESPACE_BEGIN

bool PacketParser::Parse(MemoryBuffer *&memoryBuffer) {
  while (true) {
    switch (m_state) {
      case ParserState::PacketSize: {
        if (m_service->GetServiceType() == ServiceType::Inner) {
          if (m_buffer->GetLength() < InnerPacketSizeLength) {
            memoryBuffer = nullptr;
            return false;
          }

          m_buffer->Read(std::span<byte>(m_cache, OuterPacketSizeLength));

          m_packetSize = BitConverter::ToInt(m_cache,0);
          if (m_packetSize > std::numeric_limits<ushort>::max() * 16 || m_packetSize < Packet::MinPacketSize) {
            throw Exception("recv packet Size error, 可能是外网探测端口: {}", m_packetSize);
          }
        } else {
          if (m_buffer->GetLength() < OuterPacketSizeLength) {
            memoryBuffer = nullptr;
            return false;
          }

          m_buffer->Read(std::span<byte>(m_cache, OuterPacketSizeLength));
          m_packetSize = BitConverter::ToUShort(m_cache,0);
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

CORE_NAMESPACE_END