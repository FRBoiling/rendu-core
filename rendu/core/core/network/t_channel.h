/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_T_CHANNEL_H
#define RENDU_T_CHANNEL_H

#include "a_channel.h"
#include "circular_buffer.h"
#include "error_code.h"
#include "packet_parser.h"

CORE_NAMESPACE_BEGIN

class TService;

class TChannel : public AChannel {
public:
  TChannel();

  TChannel(Long id, IPEndPoint *ipEndPoint, TService *service);

  TChannel(Long id, Socket *socket, TService *service);
  ~TChannel();

public:
  IPEndPoint *GetRemoteAddress();
  void OnComplete(void *sender, SocketAsyncEventArgs *args);
  bool IsDisposed();
public:
  void ConnectAsync();

  void OnConnectComplete(SocketAsyncEventArgs *e);

  void OnDisconnectComplete(SocketAsyncEventArgs *e);

  void StartRecv();

  void OnRecvComplete(SocketAsyncEventArgs *e);

  void HandleRecv(SocketAsyncEventArgs *e);

  void StartSend();

  void Send(MemoryBuffer *stream);

  void OnSendComplete(SocketAsyncEventArgs *eventArgs);

  void HandleSend(SocketAsyncEventArgs *eventArgs);

private:
  void OnRead(MemoryBuffer *memoryStream);
  void OnError(int error);

private:
  TService *m_service;
  Socket *m_socket;
  SocketAsyncEventArgs *m_innArgs;
  SocketAsyncEventArgs *m_outArgs;

  CircularBuffer *m_recvBuffer;
  CircularBuffer *m_sendBuffer;

  bool m_isSending;

  bool m_isConnected;

  PacketParser *m_parser;

  byte *m_sendCache;
};

CORE_NAMESPACE_END

#endif//RENDU_T_CHANNEL_H
