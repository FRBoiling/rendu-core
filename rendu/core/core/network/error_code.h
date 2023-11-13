/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_ERROR_CODE_H
#define RENDU_ERROR_CODE_H

#include "common_define.h"

RD_NAMESPACE_BEGIN

    class ErrorCore {
    public:
      static const int ERR_MyErrorCode = 110000;
      static const int ERR_KcpConnectTimeout = 100205;
      static const int ERR_KcpAcceptTimeout = 100206;
      static const int ERR_PeerDisconnect = 100208;
      static const int ERR_SocketCantSend = 100209;
      static const int ERR_SocketError = 100210;
      static const int ERR_KcpWaitSendSizeTooLarge = 100211;
      static const int ERR_KcpCreateError = 100212;
      static const int ERR_SendMessageNotFoundTChannel = 100213;
      static const int ERR_TChannelRecvError = 100214;
      static const int ERR_MessageSocketParserError = 100215;
      static const int ERR_KcpNotFoundChannel = 100216;

      static const int ERR_WebsocketSendError = 100217;
      static const int ERR_WebsocketPeerReset = 100218;
      static const int ERR_WebsocketMessageTooBig = 100219;
      static const int ERR_WebsocketRecvError = 100220;

      static const int ERR_KcpReadNotSame = 100230;
      static const int ERR_KcpSplitError = 100231;
      static const int ERR_KcpSplitCountError = 100232;

      static const int ERR_MessageLocationSenderTimeout = 110004;
      static const int ERR_PacketParserError = 110005;
      static const int ERR_KcpChannelAcceptTimeout = 110206;
      static const int ERR_KcpRemoteDisconnect = 110207;
      static const int ERR_WebsocketError = 110303;
      static const int ERR_WebsocketConnectError = 110304;
      static const int ERR_RpcFail = 110307;
      static const int ERR_ReloadFail = 110308;
      static const int ERR_ConnectGateKeyError = 110309;
      static const int ERR_SessionSendOrRecvTimeout = 110311;
      static const int ERR_OuterSessionRecvInnerMessage = 110312;
      static const int ERR_NotFoundActor = 110313;
      static const int ERR_MessageTimeout = 110315;
      static const int ERR_UnverifiedSessionSendMessage = 110316;
      static const int ERR_ActorLocationSenderTimeout2 = 110317;
      static const int ERR_ActorLocationSenderTimeout3 = 110318;
      static const int ERR_ActorLocationSenderTimeout4 = 110319;
      static const int ERR_ActorLocationSenderTimeout5 = 110320;

      static const int ERR_KcpRouterTimeout = 110401;
      static const int ERR_KcpRouterTooManyPackets = 110402;
      static const int ERR_KcpRouterSame = 110403;
      static const int ERR_KcpRouterConnectFail = 110404;
      static const int ERR_KcpRouterRouterSyncCountTooMuchTimes = 110405;
      static const int ERR_KcpRouterSyncCountTooMuchTimes = 110406;

      // 110000 以上，避免跟SocketError冲突


      //-----------------------------------

      // 小于这个Rpc会抛异常，大于这个异常的error需要自己判断处理，也就是说需要处理的错误应该要大于该值
    public:
      static const int ERR_Exception = 200000;


      static const int ERR_Cancel = 200001;

      static const int ERR_Timeout = 200002;

    public:
      static bool IsRpcNeedThrowException(int error) {
        if (error == 0) {
          return false;
        }
        // ws平台返回错误专用的值
        if (error == -1) {
          return false;
        }

        if (error > ERR_Exception) {
          return false;
        }

        return true;
      }
    };

RD_NAMESPACE_END


#endif //RENDU_ERROR_CODE_H
