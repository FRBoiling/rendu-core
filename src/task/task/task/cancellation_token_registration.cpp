/*
* Created by boil on 2024/2/10.
*/

#include "cancellation_token_registration.h"
#include "cancellation_state.h"

TASK_NAMESPACE_BEGIN

CancellationTokenRegistration::~CancellationTokenRegistration()
{
  if (m_state != nullptr)
  {
    m_state->deregister_callback(this);
    m_state->release_token_ref();
  }
}

void CancellationTokenRegistration::register_callback(CancellationToken&& token)
{
  auto* state = token.m_state;
  if (state != nullptr && state->can_be_cancelled())
  {
    m_state = state;
    if (state->try_register_callback(this))
    {
      token.m_state = nullptr;
    }
    else
    {
      m_state = nullptr;
      m_callback();
    }
  }
  else
  {
    m_state = nullptr;
  }
}



TASK_NAMESPACE_END