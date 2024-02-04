/*
* Created by boil on 2024/2/4.
*/

#include "a_logger.h"

RD_NAMESPACE_BEGIN

void ALogger::Init(std::string flag, LogLevel log_level, LogPosition log_position, LogMode log_mode) {
  SetFlag(flag);
  SetLevel(log_level);
  SetMode(log_mode);
  InitChannel();
}

void ALogger::SetLevel(LogLevel level) {
  level_ = level;
}
LogLevel ALogger::GetLevel() const {
  return level_;
}

void ALogger::SetMode(LogMode mode) {
  mode_ = mode;
}
LogMode ALogger::GetMode() const {
  return mode_;
}

void ALogger::SetFlag(STRING flag) {
  flag_ = flag;
}
STRING ALogger::GetFlag() const {
  return flag_;
}


void ALogger::SetPosition(LogPosition position) {
  position_ = position;
}

LogPosition ALogger::GetPosition() const {
  return position_;
}

void ALogger::Clean() {
  if (channels_.size() > 0) {
    channels_.clear();
  }
}

void ALogger::WriteMsg(LogLevel level, LogMsgSource prefix, const std::string &content) {
  for (auto &channel: channels_) {
    channel->WriteMsg(level, prefix, content);
  }
}

void ALogger::InitChannel() {
  AddChannel(new ALoggerChannel());
}

void ALogger::AddChannel(ALoggerChannel *new_channel) {
  bool exists = false;
  for (auto &channel: channels_) {
    if (channel->GetName() == new_channel->GetName()) {
      exists = true;
      break;
    }
  }
  if (exists) { return; }
  new_channel->Init(this);
  channels_.push_back(new_channel);
}


RD_NAMESPACE_END