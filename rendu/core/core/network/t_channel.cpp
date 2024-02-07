/*
* Created by boil on 2023/10/15.
*/

#include "t_channel.h"
#include "t_service.h"

CORE_NAMESPACE_BEGIN

TChannel::TChannel()
    : AChannel(), m_sendCache(new byte[Packet::OpcodeLength + Packet::ActorIdLength]), m_socket(
                                                                                           nullptr),
      m_innArgs(nullptr), m_outArgs(nullptr), m_recvBuffer(nullptr), m_sendBuffer(nullptr) {
}

TChannel::~TChannel() {
  Long id = m_id;
  m_id = 0;
  m_service->Remove(id, 0);
  m_socket->Close();
  delete (m_innArgs);
  delete (m_outArgs);
  delete (m_socket);
  m_innArgs = nullptr;
  m_outArgs = nullptr;
  m_socket = nullptr;
}

TChannel::TChannel(Long id, IPEndPoint *ipEndPoint, TService *service)
    : AChannel(ChannelType::Connect, *ipEndPoint, id), m_service(service) {
  m_socket = new Socket(ipEndPoint->GetAddressFamily(), SocketType::Stream, ProtocolType::Tcp);
  m_socket->SetTcpNoDelay(true);
  m_parser = new PacketParser(m_recvBuffer, (AService *) m_service);
  m_innArgs->Completed += [this](void *sender, SocketAsyncEventArgs *e) {
    OnComplete(sender, e);
  };
  m_outArgs->Completed += [this](void *sender, SocketAsyncEventArgs *e) {
    OnComplete(sender, e);
  };
  ;
  m_isConnected = false;
  m_isSending = false;

  m_service->m_queue.Enqueue(new TArgs(TcpOp::Connect, m_id));
}

TChannel::TChannel(Long id, Socket *socket, TService *service)
    : AChannel(ChannelType::Accept, socket->GetRemoteEndPoint(), id), m_service(service) {
  m_id = id;
  m_service = service;
  m_socket = socket;
  m_socket->SetTcpNoDelay(true);
  m_parser = new PacketParser(m_recvBuffer, (AService *) m_service);
  m_innArgs->Completed += [this](void *sender, SocketAsyncEventArgs *e) {
    OnComplete(sender, e);
  };
  m_outArgs->Completed += [this](void *sender, SocketAsyncEventArgs *e) {
    OnComplete(sender, e);
  };
  ;
  m_isConnected = true;
  m_isSending = false;
  m_service->m_queue.Enqueue(new TArgs(TcpOp::StartSend, m_id));
  m_service->m_queue.Enqueue(new TArgs(TcpOp::StartRecv, m_id));
}

void TChannel::OnComplete(void *sender, SocketAsyncEventArgs *args) {
  m_service->m_queue.Enqueue(new TArgs(m_id, args));
}

bool TChannel::IsDisposed() {
  return false;
}

void TChannel::Send(MemoryBuffer *stream) {
  if (IsDisposed()) {
    throw new Exception("TChannel已经被Dispose, 不能发送消息");
  }

  m_sendBuffer->Write(stream->GetBuffer(), stream->GetPosition(),
                      (int) (stream->GetLength() - stream->GetPosition()));

  if (!m_isSending) {
    //m_StartSend();
    m_service->m_queue.Enqueue(new TArgs(TcpOp::StartSend, m_id));
  }
  m_service->Recycle(stream);
}

void TChannel::ConnectAsync() {
  m_outArgs->SetRemoteEndPoint(GetRemoteAddress());
  if (m_socket->ConnectAsync(m_outArgs)) {
    return;
  }
  OnConnectComplete(m_outArgs);
}

void TChannel::OnConnectComplete(SocketAsyncEventArgs *e) {
  if (m_socket == nullptr) {
    return;
  }
  if (e->GetSocketError() != SocketError::Success) {
    OnError((int) e->GetSocketError());
    return;
  }

  e->SetRemoteEndPoint(nullptr);
  m_isConnected = true;

  m_service->m_queue.Enqueue(new TArgs(TcpOp::StartSend, m_id));
  m_service->m_queue.Enqueue(new TArgs(TcpOp::StartRecv, m_id));
}

void TChannel::OnDisconnectComplete(SocketAsyncEventArgs *e) {
  OnError((int) e->GetSocketError());
}

void TChannel::StartRecv() {
  while (true) {
    try {
      if (m_socket == nullptr) {
        return;
      }

      int size = m_recvBuffer->ChunkSize - m_recvBuffer->GetLastIndex();
      m_innArgs->SetBuffer(m_recvBuffer->GetLast(), m_recvBuffer->GetLastIndex(), size);
    } catch (Exception e) {
      RD_CRITICAL("tcp channel error: {}\n{e}", m_id, e.what());
      OnError(ErrorCore::ERR_TChannelRecvError);
      return;
    }

    if (m_socket->ReceiveAsync(m_innArgs)) {
      return;
    }
    HandleRecv(m_innArgs);
  }
}

void TChannel::OnRecvComplete(SocketAsyncEventArgs *e) {
  HandleRecv(e);

  if (m_socket == nullptr) {
    return;
  }
  m_service->m_queue.Enqueue(new TArgs(TcpOp::StartRecv, m_id));
}

void TChannel::HandleRecv(SocketAsyncEventArgs *e) {
  if (m_socket == nullptr) {
    return;
  }
  if (e->GetSocketError() != SocketError::Success) {
    OnError((int) e->GetSocketError());
    return;
  }

  if (e->GetBytesTransferred() == 0) {
    OnError(ErrorCore::ERR_PeerDisconnect);
    return;
  }

  m_recvBuffer->SetLastIndex((int) m_recvBuffer->GetLastIndex() + e->GetBytesTransferred());
  if (m_recvBuffer->GetLastIndex() == m_recvBuffer->ChunkSize) {
    m_recvBuffer->AddLast();
    m_recvBuffer->SetLastIndex(0);
  }

  // 收到消息回调
  while (true) {
    // 这里循环解析消息执行，有可能，执行消息的过程中断开了session
    if (m_socket == nullptr) {
      return;
    }
    try {
      MemoryBuffer *memoryBuffer = nullptr;
      bool ret = m_parser->Parse(memoryBuffer);
      if (!ret) {
        break;
      }
      OnRead(memoryBuffer);
      m_service->Recycle(memoryBuffer);
    } catch (Exception &ex) {
      RD_CRITICAL("ip: {} {}", GetRemoteAddress()->ToString(), ex.what());
      OnError(ErrorCore::ERR_SocketError);
      return;
    }
  }
}

void TChannel::StartSend() {
  if (!m_isConnected) {
    return;
  }

  if (m_isSending) {
    return;
  }

  while (true) {
    try {
      if (m_socket == nullptr) {
        m_isSending = false;
        return;
      }

      // 没有数据需要发送
      if (m_sendBuffer->GetLength() == 0) {
        m_isSending = false;
        return;
      }

      m_isSending = true;

      int sendSize = m_sendBuffer->ChunkSize - m_sendBuffer->GetFirstIndex();
      if (sendSize > m_sendBuffer->GetLength()) {
        sendSize = (int) m_sendBuffer->GetLength();
      }

      m_outArgs->SetBuffer(m_sendBuffer->GetFirst(), m_sendBuffer->GetFirstIndex(), sendSize);

      if (m_socket->SendAsync(m_outArgs)) {
        return;
      }

      HandleSend(m_outArgs);
    } catch (Exception &ex) {
      throw Exception("socket set buffer error: {}, {}\n{}", sizeof(m_sendBuffer->GetFirst()),
                      m_sendBuffer->GetFirstIndex(), ex.what());
    }
  }
}

void TChannel::OnSendComplete(SocketAsyncEventArgs *eventArgs) {
  HandleSend(eventArgs);
  m_isSending = false;
  m_service->m_queue.Enqueue(new TArgs(TcpOp::StartSend, m_id));
}


void TChannel::OnRead(MemoryBuffer *memoryStream) {
  try {
    m_service->OnReadCallback(m_id, memoryStream);
  } catch (Exception &e) {
    RD_CRITICAL("TChannel OnRead: exception {} {}", e.what());
    OnError(ErrorCore::ERR_PacketParserError);
  }
}

void TChannel::OnError(int error) {
  RD_INFO("TChannel OnError: {} {}", error, GetRemoteAddress()->ToString());
  Long channelId = m_id;
  m_service->Remove(channelId, error);
  m_service->OnErrorCallback(channelId, error);
}

void TChannel::HandleSend(SocketAsyncEventArgs *eventArgs) {
  if (m_socket == nullptr) {
    return;
  }
  if (eventArgs->GetSocketError() != SocketError::Success) {
    OnError((int) eventArgs->GetSocketError());
    return;
  }

  if (eventArgs->GetBytesTransferred() == 0) {
    OnError(ErrorCore::ERR_PeerDisconnect);
    return;
  }

  m_sendBuffer->SetFirstIndex(m_sendBuffer->GetFirstIndex() + eventArgs->GetBytesTransferred());
  if (m_sendBuffer->GetFirstIndex() == m_sendBuffer->ChunkSize) {
    m_sendBuffer->SetFirstIndex(0);
    m_sendBuffer->RemoveFirst();
  }
}


CORE_NAMESPACE_END
