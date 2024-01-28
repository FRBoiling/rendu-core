/*
* Created by boil on 2023/10/15.
*/

#include "t_service.h"
#include "exception/argument_out_of_range_exception.h"
#include "exception/exception_helper.h"
#include "network/net_work_helper.h"

RD_NAMESPACE_BEGIN

    TService::TService(AddressFamily addressFamily, ServiceType serviceType) {
      m_serviceType = serviceType;
    }

    TService::TService(IPEndPoint &ipEndPoint, ServiceType serviceType) : m_innArgs(new SocketAsyncEventArgs()){
      m_acceptor = new Socket(ipEndPoint.GetAddressFamily(), SocketType::Stream, ProtocolType::Tcp);
      // 容易出问题，先注释掉，按需开启
      //m_acceptor.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
      m_innArgs->Completed += [this](auto &&PH1, auto &&PH2) {
        OnComplete(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
      };

      try {
        m_acceptor->Bind(ipEndPoint);
      }
      catch (Exception &ex) {
        throw Exception("bind error: {}\n{}", ipEndPoint.to_string(), ex.what());
      }
      m_acceptor->Listen(1000);
      AcceptAsync();
    }

    TService::~TService() {
      m_acceptor->Close();
      m_acceptor = nullptr;
      delete (m_innArgs);
      m_idChannels.Clear();
    }

    void TService::Create(int64 id, string address) {
      TChannel *channel = nullptr;
      if (m_idChannels.TryGetValue(id, channel)) {
        return;
      }

      IPEndPoint *endPoint = NetworkHelper::ToIPEndPoint(address);
      channel = new TChannel(id, endPoint, this);
      m_idChannels.Add(channel->GetId(), channel);
    }

    void TService::Send(int64 channelId, MemoryBuffer *memoryBuffer) {
      try {
        TChannel *channel = Get(channelId);
        if (channel == nullptr) {
          OnErrorCallback(channelId, ErrorCore::ERR_SendMessageNotFoundTChannel);
          return;
        }

        channel->Send(memoryBuffer);
      }
      catch (Exception &ex) {
        RD_CRITICAL(ex.what());
      }
    }

    void TService::Remove(int64 id, int error) {
      TChannel *channel = nullptr;
      if (m_idChannels.TryGetValue(id, channel)) {
        channel->SetError(error);
      }
      m_idChannels.Remove(id);
      delete (channel);
    }

    void TService::Update() {
      while (true) {
        TArgs *args = nullptr;
        if (!m_queue.TryDequeue(args)) {
          break;
        }
        SocketAsyncEventArgs *eventArgs = args->SocketAsyncEventArgs;
        if (eventArgs == nullptr) {
          switch (args->Op) {
            case TcpOp::StartSend: {
              TChannel *tChannel = Get(args->ChannelId);
              if (tChannel != nullptr) {
                tChannel->StartSend();
              }
              break;
            }
            case TcpOp::StartRecv: {
              TChannel *tChannel = Get(args->ChannelId);
              if (tChannel != nullptr) {
                tChannel->StartRecv();
              }
              break;
            }
            case TcpOp::Connect: {
              TChannel *tChannel = Get(args->ChannelId);
              if (tChannel != nullptr) {
                tChannel->ConnectAsync();
              }
              break;
            }
          }
          continue;
        }

        switch (eventArgs->GetLastOperation()) {
          case SocketAsyncOperation::Accept: {
            SocketError socketError = eventArgs->GetSocketError();
            Socket *acceptSocket = eventArgs->GetAcceptSocket();
            OnAcceptComplete(socketError, acceptSocket);
            break;
          }
          case SocketAsyncOperation::Connect: {
            TChannel *tChannel = Get(args->ChannelId);
            if (tChannel != nullptr) {
              tChannel->OnConnectComplete(eventArgs);
            }

            break;
          }
          case SocketAsyncOperation::Disconnect: {
            TChannel *tChannel = Get(args->ChannelId);
            if (tChannel != nullptr) {
              tChannel->OnDisconnectComplete(eventArgs);
            }
            break;
          }
          case SocketAsyncOperation::Receive: {
            TChannel *tChannel = Get(args->ChannelId);
            if (tChannel != nullptr) {
              tChannel->OnRecvComplete(eventArgs);
            }
            break;
          }
          case SocketAsyncOperation::Send: {
            TChannel *tChannel = Get(args->ChannelId);
            if (tChannel != nullptr) {
              tChannel->OnSendComplete(eventArgs);
            }
            break;
          }
          default:
            throw ArgumentOutOfRangeException("Update:\n {}", (int32) eventArgs->GetLastOperation());
        }
      }
    }


    void TService::OnComplete(void *sender, SocketAsyncEventArgs *eventArgs) {
      switch (eventArgs->GetLastOperation()) {
        case SocketAsyncOperation::Accept:
          m_queue.Enqueue(new TArgs(eventArgs));
          break;
        default:
          throw Exception("socket error: {}", (int) eventArgs->GetLastOperation());
      }
    }

    void TService::OnAcceptComplete(SocketError socketError, Socket *acceptSocket) {
      if (m_acceptor == nullptr) {
        return;
      }

      if (socketError != SocketError::Success) {
        RD_CRITICAL("accept error {}", (int32) socketError);
        AcceptAsync();
        return;
      }

      try {
        int64 id = NetServices::Instance().CreateAcceptChannelId();
        TChannel *channel = new TChannel(id, acceptSocket, this);
        m_idChannels.Add(channel->GetId(), channel);
        int64 channelId = channel->GetId();

        OnAcceptCallback(channelId, channel->GetRemoteAddress());
      }
      catch (Exception &ex) {
        RD_CRITICAL(ex.what());
      }
      // 开始新的accept
      AcceptAsync();
    }


    TChannel *TService::Get(int64 id) {
      TChannel *channel = nullptr;
      m_idChannels.TryGetValue(id, channel);
      return channel;
    }

    void TService::AcceptAsync() {
      m_innArgs->m_AcceptSocket = nullptr;
      if (m_acceptor->AcceptAsync(m_innArgs)) {
        return;
      }
      OnAcceptComplete(m_innArgs->GetSocketError(), m_innArgs->m_AcceptSocket);
    }

    void TService::Dispose() {
      AService::Dispose();
    }

    bool TService::IsDisposed() {
      return false;
    }

    std::tuple<uint32_t, uint32_t> TService::GetChannelConn(int64 channelId) {
      return AService::GetChannelConn(channelId);
    }

    void TService::ChangeAddress(int64 channelId, IPEndPoint ipEndPoint) {
      AService::ChangeAddress(channelId, ipEndPoint);
    }


RD_NAMESPACE_END