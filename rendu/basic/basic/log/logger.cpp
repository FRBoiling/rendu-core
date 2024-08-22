/*
* Created by boil on 2024/8/9.
*/

#include "logger.h"

RD_NAMESPACE_BEGIN

void Logger::init(String flag, LogLevel log_level, LogPosition log_position, LogMode log_mode) {
  setFlag(flag);
  setLevel(log_level);
  setMode(log_mode);
}

void Logger::setLevel(LogLevel level) {
  level_ = level;
}
LogLevel Logger::getLevel() const {
  return level_;
}

void Logger::setMode(LogMode mode) {
  mode_ = mode;
}
LogMode Logger::getMode() const {
  return mode_;
}

void Logger::setFlag(String flag) {
  flag_ = flag;
}
String Logger::getFlag() const {
  return flag_;
}


void Logger::setPosition(LogPosition position) {
  position_ = position;
}

LogPosition Logger::getPosition() const {
  return position_;
}

void Logger::clean() {
  if (channels_.size() > 0) {
    channels_.clear();
  }
}

void Logger::write(LogMsgSource prefix, LogMsg content) {
  if (channels_.size() == 0) {
    addChannel(new LoggerChannel(this, "default channel"));
  }
  for (auto channel: channels_) {
    channel->write(level_, prefix, content);
  }
}

void Logger::addChannel(LoggerChannel *new_channel) {
  bool exists = false;
  for (auto &channel: channels_) {
    if (channel->getName() == new_channel->getName()) {
      exists = true;
      break;
    }
  }
  if (exists) { return; }
  channels_.push_back(new_channel);
}

RD_NAMESPACE_END
