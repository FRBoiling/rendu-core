/*
* Created by boil on 2024/2/10.
*/

#ifndef RENDU_TASK_TASK_TASK_CancellationToken_H_
#define RENDU_TASK_TASK_TASK_CancellationToken_H_

#include "task_define.h"

TASK_NAMESPACE_BEGIN

  class CancellationSource;
  class CancellationTokenRegistration;

  namespace detail
  {
    class cancellation_state;
  }

  class CancellationToken
  {
  public:

    /// Construct to a cancellation token that can't be cancelled.
    CancellationToken() noexcept;

    /// Copy another cancellation token.
    ///
    /// New token will refer to the same underlying state.
    CancellationToken(const CancellationToken& other) noexcept;

    CancellationToken(CancellationToken&& other) noexcept;

    ~CancellationToken();

    CancellationToken& operator=(const CancellationToken& other) noexcept;

    CancellationToken& operator=(CancellationToken&& other) noexcept;

    void swap(CancellationToken& other) noexcept;

    /// Query if it is possible that this operation will be cancelled
    /// or not.
    ///
    /// Cancellable operations may be able to take more efficient code-paths
    /// if they don't need to handle cancellation requests.
    bool can_be_cancelled() const noexcept;

    /// Query if some thread has requested cancellation on an associated
    /// CancellationSource object.
    bool is_cancellation_requested() const noexcept;

    /// Throws cppcoro::operation_cancelled exception if cancellation
    /// has been requested for the associated operation.
    void throw_if_cancellation_requested() const;

  private:

    friend class CancellationSource;
    friend class CancellationTokenRegistration;

    CancellationToken(detail::cancellation_state* state) noexcept;

    detail::cancellation_state* m_state;

  };

  inline void swap(CancellationToken& a, CancellationToken& b) noexcept
  {
    a.swap(b);
  }


TASK_NAMESPACE_END
#endif//RENDU_TASK_TASK_TASK_CancellationToken_H_
