/*
* Created by boil on 2024/2/10.
*/

#include "cancellation_token.h"
#include "cancellation_state.h"
#include "exception/operation_cancelled_exception.h"

TASK_NAMESPACE_BEGIN


CancellationToken::CancellationToken() noexcept
    : m_state(nullptr)
{
}

CancellationToken::CancellationToken(const CancellationToken& other) noexcept
    : m_state(other.m_state)
{
  if (m_state != nullptr)
  {
    m_state->add_token_ref();
  }
}

CancellationToken::CancellationToken(CancellationToken&& other) noexcept
    : m_state(other.m_state)
{
  other.m_state = nullptr;
}

CancellationToken::~CancellationToken()
{
  if (m_state != nullptr)
  {
    m_state->release_token_ref();
  }
}

CancellationToken& CancellationToken::operator=(const CancellationToken& other) noexcept
{
  if (other.m_state != m_state)
  {
    if (m_state != nullptr)
    {
      m_state->release_token_ref();
    }

    m_state = other.m_state;

    if (m_state != nullptr)
    {
      m_state->add_token_ref();
    }
  }

  return *this;
}

CancellationToken& CancellationToken::operator=(CancellationToken&& other) noexcept
{
  if (this != &other)
  {
    if (m_state != nullptr)
    {
      m_state->release_token_ref();
    }

    m_state = other.m_state;
    other.m_state = nullptr;
  }

  return *this;
}

void CancellationToken::swap(CancellationToken& other) noexcept
{
  std::swap(m_state, other.m_state);
}

bool CancellationToken::can_be_cancelled() const noexcept
{
  return m_state != nullptr && m_state->can_be_cancelled();
}

bool CancellationToken::is_cancellation_requested() const noexcept
{
  return m_state != nullptr && m_state->is_cancellation_requested();
}

void CancellationToken::throw_if_cancellation_requested() const
{
  if (is_cancellation_requested())
  {
    throw OperationCancelledException();
  }
}

CancellationToken::CancellationToken(detail::cancellation_state* state) noexcept
    : m_state(state)
{
  if (m_state != nullptr)
  {
    m_state->add_token_ref();
  }
}



TASK_NAMESPACE_END