/*
* Created by boil on 2024/2/10.
*/

#ifndef RENDU_TASK_TASK_TASK_TASK_CANCELLATION_REGISTRATION_H_
#define RENDU_TASK_TASK_TASK_TASK_CANCELLATION_REGISTRATION_H_


#include "task_define.h"
#include "cancellation_token.h"

TASK_NAMESPACE_BEGIN
namespace detail {
  class cancellation_state;
  struct cancellation_registration_list_chunk;
  struct cancellation_registration_state;
}// namespace detail

class CancellationTokenRegistration {
public:
  /// Registers the callback to be executed when cancellation is requested
  /// on the CancellationToken.
  ///
  /// The callback will be executed if cancellation is requested for the
  /// specified cancellation token. If cancellation has already been requested
  /// then the callback will be executed immediately, before the constructor
  /// returns. If cancellation has not yet been requested then the callback
  /// will be executed on the first thread to request cancellation inside
  /// the call to cancellation_source::request_cancellation().
  ///
  /// \param token
  /// The cancellation token to register the callback with.
  ///
  /// \param callback
  /// The callback to be executed when cancellation is requested on the
  /// the CancellationToken. Note that callback must not throw an exception
  /// if called when cancellation is requested otherwise std::terminate()
  /// will be called.
  ///
  /// \throw std::bad_alloc
  /// If registration failed due to insufficient memory available.
  template<
      typename FUNC,
      typename = std::enable_if_t<std::is_constructible_v<std::function<void()>, FUNC &&>>>
  CancellationTokenRegistration(CancellationToken token, FUNC &&callback)
      : m_callback(std::forward<FUNC>(callback)) {
    register_callback(std::move(token));
  }

  CancellationTokenRegistration(const CancellationTokenRegistration &other) = delete;
  CancellationTokenRegistration &operator=(const CancellationTokenRegistration &other) = delete;

  /// Deregisters the callback.
  ///
  /// After the destructor returns it is guaranteed that the callback
  /// will not be subsequently called during a call to request_cancellation()
  /// on the cancellation_source.
  ///
  /// This may block if cancellation has been requested on another thread
  /// is it will need to wait until this callback has finished executing
  /// before the callback can be destroyed.
  ~CancellationTokenRegistration();

private:
  friend class detail::cancellation_state;
  friend struct detail::cancellation_registration_state;

  void register_callback(CancellationToken &&token);

  detail::cancellation_state *m_state;
  std::function<void()> m_callback;
  detail::cancellation_registration_list_chunk *m_chunk;
  std::uint32_t m_entryIndex;
};

TASK_NAMESPACE_END


#endif//RENDU_TASK_TASK_TASK_TASK_CANCELLATION_REGISTRATION_H_
