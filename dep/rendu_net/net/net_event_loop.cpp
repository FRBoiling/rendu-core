/*
* Created by boil on 2023/11/10.
*/

#include "net_event_loop.h"

RD_NAMESPACE_BEGIN
  NetEventLoop::NetEventLoop() {

  }

  NetEventLoop::~NetEventLoop() {

  }


  void NetEventLoop::UpdateChannel(NetChannel *channel) {

  }

  void NetEventLoop::AddChannel(NetChannel *channel) {
  }

  void NetEventLoop::RemoveChannel(NetChannel *channel) {
  }

  const int kPollTimeMs = 10000;

  void NetEventLoop::Loop() {
    while (!quit_) {
      _activeChannels.clear();
      _pollReturnTime = _poller->Poll(kPollTimeMs, _activeChannels);
      // TODO sort channel by priority
      for (NetChannel *channel: _activeChannels) {
        _currentActiveChannel = channel;
        _currentActiveChannel->HandleEvent(_pollReturnTime);
      }
      _currentActiveChannel = nullptr;
    }
  }

  void NetEventLoop::Quit() {

  }


RD_NAMESPACE_END

