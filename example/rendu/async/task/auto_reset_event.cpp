///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////

#include "auto_reset_event.hpp"

ASYNC_NAMESPACE_BEGIN

auto_reset_event::auto_reset_event(bool initiallySet)
    : m_isSet(initiallySet) {}

auto_reset_event::~auto_reset_event() {}

void auto_reset_event::set() {
  std::unique_lock lock{m_mutex};
  if (!m_isSet) {
    m_isSet = true;
    m_cv.notify_one();
  }
}

void auto_reset_event::wait() {
  std::unique_lock lock{m_mutex};
  while (!m_isSet) {
    m_cv.wait(lock);
  }
  m_isSet = false;
}

ASYNC_NAMESPACE_END