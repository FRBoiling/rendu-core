/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_ADDRESS_FAMILY_H
#define RENDU_ADDRESS_FAMILY_H



#include "define.h"

RD_NAMESPACE_BEGIN

    enum class AddressFamily {
      /// <summary><para>Unknown GetAddress family.</para></summary>
      Unknown = -1, // 0xFFFFFFFF
      /// <summary><para>Unspecified GetAddress family.</para></summary>
      Unspecified = 0,
      /// <summary><para>Unix local to host GetAddress.</para></summary>
      Unix = 1,
      /// <summary><para>Address for IP version 4.</para></summary>
      InterNetwork = 2,
      /// <summary><para>ARPANET IMP GetAddress.</para></summary>
      ImpLink = 3,
      /// <summary><para>Address for PUP protocols.</para></summary>
      Pup = 4,
      /// <summary><para>Address for MIT CHAOS protocols.</para></summary>
      Chaos = 5,
      /// <summary><para>IPX or SPX GetAddress.</para></summary>
      Ipx = 6,
      /// <summary><para>Address for Xerox NS protocols.</para></summary>
      NS = 6,
      /// <summary><para>Address for ISO protocols.</para></summary>
      Iso = 7,
      /// <summary><para>Address for OSI protocols.</para></summary>
      Osi = 7,
      /// <summary><para>European Computer Manufacturers Association (ECMA) GetAddress.</para></summary>
      Ecma = 8,
      /// <summary><para>Address for Datakit protocols.</para></summary>
      DataKit = 9,
      /// <summary><para>Addresses for CCITT protocols, such as X.25.</para></summary>
      Ccitt = 10, // 0x0000000A
      /// <summary><para>IBM SNA GetAddress.</para></summary>
      Sna = 11, // 0x0000000B
      /// <summary><para>DECnet GetAddress.</para></summary>
      DecNet = 12, // 0x0000000C
      /// <summary><para>Direct Data-link interface GetAddress.</para></summary>
      DataLink = 13, // 0x0000000D
      /// <summary><para>LAT GetAddress.</para></summary>
      Lat = 14, // 0x0000000E
      /// <summary><para>NSC Hyperchannel GetAddress.</para></summary>
      HyperChannel = 15, // 0x0000000F
      /// <summary><para>AppleTalk GetAddress.</para></summary>
      AppleTalk = 16, // 0x00000010
      /// <summary><para>NetBios GetAddress.</para></summary>
      NetBios = 17, // 0x00000011
      /// <summary><para>VoiceView GetAddress.</para></summary>
      VoiceView = 18, // 0x00000012
      /// <summary><para>FireFox GetAddress.</para></summary>
      FireFox = 19, // 0x00000013
      /// <summary><para>Banyan GetAddress.</para></summary>
      Banyan = 21, // 0x00000015
      /// <summary><para>Native ATM services GetAddress.</para></summary>
      Atm = 22, // 0x00000016
      /// <summary><para>Address for IP version 6.</para></summary>
      InterNetworkV6 = 23, // 0x00000017
      /// <summary><para>Address for Microsoft cluster products.</para></summary>
      Cluster = 24, // 0x00000018
      /// <summary><para>IEEE 1284.4 workgroup GetAddress.</para></summary>
      Ieee12844 = 25, // 0x00000019
      /// <summary><para>IrDA GetAddress.</para></summary>
      Irda = 26, // 0x0000001A
      /// <summary><para>Address for Network Designers OSI gateway-enabled protocols.</para></summary>
      NetworkDesigners = 28, // 0x0000001C
      /// <summary><para>MAX GetAddress.</para></summary>
      Max = 29, // 0x0000001D
    };

RD_NAMESPACE_END

#endif //RENDU_ADDRESS_FAMILY_H
