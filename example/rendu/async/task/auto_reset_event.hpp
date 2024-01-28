/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_AUTO_RESET_EVENT_HPP
#define RENDU_ASYNC_AUTO_RESET_EVENT_HPP

#include "async_define.h"

#include <condition_variable>
#include <mutex>

ASYNC_NAMESPACE_BEGIN

class auto_reset_event {
public:
  auto_reset_event(bool initiallySet = false);

  ~auto_reset_event();

  void set();

  void wait();

private:
  std::mutex m_mutex;
  std::condition_variable m_cv;
  bool m_isSet;
};

ASYNC_NAMESPACE_END

#endif
