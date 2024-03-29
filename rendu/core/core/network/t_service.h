/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_T_SERVICE_H
#define RENDU_T_SERVICE_H

#include "a_service.h"
#include "base/utils/singleton.h"
#include "net_services.h"
#include "t_channel.h"

RD_NAMESPACE_BEGIN

    enum class TcpOp {
      StartSend,
      StartRecv,
      Connect,
    };

    struct TArgs {
    public:
      TArgs(TcpOp op, INT64 channel_id) : Op(op), ChannelId(channel_id), SocketAsyncEventArgs(nullptr) {
      }

      TArgs(INT64 channel_id, SocketAsyncEventArgs *pArgs) : Op(TcpOp::Connect), ChannelId(channel_id),
                                                             SocketAsyncEventArgs(pArgs) {
      }

      TArgs(SocketAsyncEventArgs *pArgs) : SocketAsyncEventArgs(pArgs) {
      }

    public:
      TcpOp Op;
      INT64 ChannelId;
      SocketAsyncEventArgs *SocketAsyncEventArgs;
    };

    class TService : public AService {

    public:
      TService(AddressFamily addressFamily, ServiceType serviceType);

      TService(IPEndPoint &ipEndPoint, ServiceType serviceType);

      ~TService();

    public:
      void Create(INT64 id, string address) override;

      void Send(INT64 channelId, MemoryBuffer *memoryBuffer) override;

      void Remove(INT64 id, int error) override;

      void Update() override;

    public:
      void OnComplete(void *sender, SocketAsyncEventArgs *eventArgs);

      void OnAcceptComplete(SocketError socketError, Socket *acceptSocket);

      void Dispose() override;

      bool IsDisposed() override;

      std::tuple<uint32_t, uint32_t> GetChannelConn(INT64 channelId) override;

      void ChangeAddress(INT64 channelId, IPEndPoint ipEndPoint) override;

    private:
      TChannel *Get(INT64 id);

      void AcceptAsync();

    public:
      ConcurrentQueue<TArgs *> m_queue;
    private:
      Dictionary<INT64, TChannel *> m_idChannels;

      SocketAsyncEventArgs *m_innArgs{};

      Socket *m_acceptor{};

    };

RD_NAMESPACE_END

#endif //RENDU_T_SERVICE_H
