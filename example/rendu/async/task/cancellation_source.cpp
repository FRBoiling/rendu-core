/*
* Created by boil on 2024/01/01.
*/

#include "cancellation_source.hpp"

#include "cancellation_state.hpp"

#include <cassert>

ASYNC_NAMESPACE_BEGIN

cancellation_source::cancellation_source()
    : m_state(detail::cancellation_state::create()) {
}

cancellation_source::cancellation_source(const cancellation_source &other) noexcept
    : m_state(other.m_state) {
  if (m_state != nullptr) {
    m_state->add_source_ref();
  }
}

cancellation_source::cancellation_source(cancellation_source &&other) noexcept
    : m_state(other.m_state) {
  other.m_state = nullptr;
}

cancellation_source::~cancellation_source() {
  if (m_state != nullptr) {
    m_state->release_source_ref();
  }
}

cancellation_source &cancellation_source::operator=(const cancellation_source &other) noexcept {
  if (m_state != other.m_state) {
    if (m_state != nullptr) {
      m_state->release_source_ref();
    }

    m_state = other.m_state;

    if (m_state != nullptr) {
      m_state->add_source_ref();
    }
  }

  return *this;
}

cancellation_source &cancellation_source::operator=(cancellation_source &&other) noexcept {
  if (this != &other) {
    if (m_state != nullptr) {
      m_state->release_source_ref();
    }

    m_state = other.m_state;
    other.m_state = nullptr;
  }

  return *this;
}

bool cancellation_source::can_be_cancelled() const noexcept {
  return m_state != nullptr;
}

cancellation_token cancellation_source::token() const noexcept {
  return cancellation_token(m_state);
}

void cancellation_source::request_cancellation() {
  if (m_state != nullptr) {
    m_state->request_cancellation();
  }
}

bool cancellation_source::is_cancellation_requested() const noexcept {
  return m_state != nullptr && m_state->is_cancellation_requested();
}

ASYNC_NAMESPACE_END