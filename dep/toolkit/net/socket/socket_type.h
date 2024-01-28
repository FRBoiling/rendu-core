/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_SOCKET_TYPE_H
#define RENDU_SOCKET_TYPE_H

#include "define.h"

RD_NAMESPACE_BEGIN

    enum class SocketType {
      /// <summary>Specifies an unknown <see cref="T:System.Net.Sockets.Socket" /> type.</summary>
      Unknown = -1, // 0xFFFFFFFF
      /// <summary>Supports reliable, two-way, connection-based byte streams without the duplication of Data and without preservation of boundaries. A <see cref="T:System.Net.Sockets.Socket" /> of this type communicates with a single peer and requires a remote host connection before communication can begin. <see cref="F:System.Net.Sockets.SocketType.Stream" /> uses the Transmission Control Protocol (<see langword="ProtocolType" />.<see cref="F:System.Net.Sockets.ProtocolType.Tcp" />) and the <see langword="AddressFamily" />.<see cref="F:System.Net.Sockets.AddressFamily.InterNetwork" /> GetAddress family.</summary>
      Stream = 1,
      /// <summary>Supports datagrams, which are connectionless, unreliable messages of a fixed (typically small) maximum length. Messages might be lost or duplicated and might arrive out of order. A <see cref="T:System.Net.Sockets.Socket" /> of type <see cref="F:System.Net.Sockets.SocketType.Dgram" /> requires no connection prior to sending and receiving Data, and can communicate with multiple peers. <see cref="F:System.Net.Sockets.SocketType.Dgram" /> uses the Datagram Protocol (<see langword="ProtocolType" />.<see cref="F:System.Net.Sockets.ProtocolType.Udp" />) and the <see langword="AddressFamily" />.<see cref="F:System.Net.Sockets.AddressFamily.InterNetwork" /> GetAddress family.</summary>
      Dgram = 2,
      /// <summary>Supports access to the underlying transport protocol. Using <see cref="F:System.Net.Sockets.SocketType.Raw" />, you can communicate using protocols like Internet Control Message Protocol (<see langword="ProtocolType" />.<see cref="F:System.Net.Sockets.ProtocolType.Icmp" />) and Internet Group Management Protocol (<see langword="ProtocolType" />.<see cref="F:System.Net.Sockets.ProtocolType.Igmp" />). Your application must provide a complete IP header when sending. Received datagrams return with the IP header and options intact.</summary>
      Raw = 3,
      /// <summary>Supports connectionless, message-oriented, reliably delivered messages, and preserves message boundaries in data. Rdm (Reliably Delivered Messages) messages arrive unduplicated and in order. Furthermore, the sender is notified if messages are lost. If you initialize a <see cref="T:System.Net.Sockets.Socket" /> using <see cref="F:System.Net.Sockets.SocketType.Rdm" />, you do not require a remote host connection before sending and receiving Data. With <see cref="F:System.Net.Sockets.SocketType.Rdm" />, you can communicate with multiple peers.</summary>
      Rdm = 4,
      /// <summary>Provides connection-oriented and reliable two-way transfer of ordered byte streams across a network. <see cref="F:System.Net.Sockets.SocketType.Seqpacket" /> does not duplicate data, and it preserves boundaries within the Data stream. A <see cref="T:System.Net.Sockets.Socket" /> of type <see cref="F:System.Net.Sockets.SocketType.Seqpacket" /> communicates with a single peer and requires a remote host connection before communication can begin.</summary>
      Seqpacket = 5,
    };


    enum class SocketAsyncOperation
    {
      /// <summary><para>None of the socket operations.</para></summary>
      None,
      /// <summary><para>A socket Accept operation. </para></summary>
      Accept,
      /// <summary><para>A socket Connect operation.</para></summary>
      Connect,
      /// <summary><para>A socket Disconnect operation.</para></summary>
      Disconnect,
      /// <summary><para>A socket Receive operation.</para></summary>
      Receive,
      /// <summary><para>A socket ReceiveFrom operation.</para></summary>
      ReceiveFrom,
      /// <summary><para>A socket ReceiveMessageFrom operation.</para></summary>
      ReceiveMessageFrom,
      /// <summary><para>A socket Send operation.</para></summary>
      Send,
      /// <summary><para>A socket SendPackets operation.</para></summary>
      SendPackets,
      /// <summary><para>A socket SendTo operation.</para></summary>
      SendTo,
    };

RD_NAMESPACE_END

#endif //RENDU_SOCKET_TYPE_H
