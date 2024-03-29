/*
* Created by boil on 2024/01/01.
*/


#include "cancellation_token.hpp"
#include "operation_cancelled.hpp"

#include "cancellation_state.hpp"

#include <cassert>
#include <utility>

ASYNC_NAMESPACE_BEGIN

cancellation_token::cancellation_token() noexcept
    : m_state(nullptr) {
}

cancellation_token::cancellation_token(const cancellation_token &other) noexcept
    : m_state(other.m_state) {
  if (m_state != nullptr) {
    m_state->add_token_ref();
  }
}

cancellation_token::cancellation_token(cancellation_token &&other) noexcept
    : m_state(other.m_state) {
  other.m_state = nullptr;
}

cancellation_token::~cancellation_token() {
  if (m_state != nullptr) {
    m_state->release_token_ref();
  }
}

cancellation_token &cancellation_token::operator=(const cancellation_token &other) noexcept {
  if (other.m_state != m_state) {
    if (m_state != nullptr) {
      m_state->release_token_ref();
    }

    m_state = other.m_state;

    if (m_state != nullptr) {
      m_state->add_token_ref();
    }
  }

  return *this;
}

cancellation_token &cancellation_token::operator=(cancellation_token &&other) noexcept {
  if (this != &other) {
    if (m_state != nullptr) {
      m_state->release_token_ref();
    }

    m_state = other.m_state;
    other.m_state = nullptr;
  }

  return *this;
}

void cancellation_token::swap(cancellation_token &other) noexcept {
  std::swap(m_state, other.m_state);
}

bool cancellation_token::can_be_cancelled() const noexcept {
  return m_state != nullptr && m_state->can_be_cancelled();
}

bool cancellation_token::is_cancellation_requested() const noexcept {
  return m_state != nullptr && m_state->is_cancellation_requested();
}

void cancellation_token::throw_if_cancellation_requested() const {
  if (is_cancellation_requested()) {
    throw operation_cancelled{};
  }
}

cancellation_token::cancellation_token(detail::cancellation_state *state) noexcept
    : m_state(state) {
  if (m_state != nullptr) {
    m_state->add_token_ref();
  }
}

ASYNC_NAMESPACE_END