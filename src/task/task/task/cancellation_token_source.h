/*
* Created by boil on 2024/2/10.
*/

#ifndef RENDU_TASK_TASK_TASK_CancellationToken_SOURCE_H_
#define RENDU_TASK_TASK_TASK_CancellationToken_SOURCE_H_

#include "task_define.h"

TASK_NAMESPACE_BEGIN

class CancellationToken;

namespace detail {
  class cancellation_state;
}

class CancellationTokenSource {
public:
  /// Construct to a new cancellation source.
  CancellationTokenSource();

  /// Create a new reference to the same underlying cancellation
  /// source as \p other.
  CancellationTokenSource(const CancellationTokenSource &other) noexcept;

  CancellationTokenSource(CancellationTokenSource &&other) noexcept;

  ~CancellationTokenSource();

  CancellationTokenSource &operator=(const CancellationTokenSource &other) noexcept;

  CancellationTokenSource &operator=(CancellationTokenSource &&other) noexcept;

  /// Query if this cancellation source can be cancelled.
  ///
  /// A cancellation source object will not be cancellable if it has
  /// previously been moved into another CancellationTokenSource instance
  /// or was copied from a CancellationTokenSource that was not cancellable.
  bool can_be_cancelled() const noexcept;

  /// Obtain a cancellation token that can be used to query if
  /// cancellation has been requested on this source.
  ///
  /// The cancellation token can be passed into functions that you
  /// may want to later be able to request cancellation.
  CancellationToken token() const noexcept;

  /// Request cancellation of operations that were passed an associated
  /// cancellation token.
  ///
  /// Any cancellation callback registered via a cancellation_registration
  /// object will be called inside this function by the first thread to
  /// call this method.
  ///
  /// This operation is a no-op if can_be_cancelled() returns false.
  void request_cancellation();

  /// Query if some thread has called 'request_cancellation()' on this
  /// CancellationTokenSource.
  bool is_cancellation_requested() const noexcept;

private:
  detail::cancellation_state *m_state;
};

TASK_NAMESPACE_END

#endif//RENDU_TASK_TASK_TASK_CancellationToken_SOURCE_H_
