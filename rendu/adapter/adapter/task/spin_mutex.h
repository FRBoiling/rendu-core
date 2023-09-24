/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_SPIN_MUTEX_H
#define RENDU_SPIN_MUTEX_H

#include "define.h"
# include <mutex>
# include <condition_variable>

RD_NAMESPACE_BEGIN

class spin_mutex
{
public:

  /// Initialise the mutex to the unlocked state.
  spin_mutex() noexcept;

  /// Attempt to lock the mutex without blocking
  ///
  /// \return
  /// true if the lock was acquired, false if the lock was already held
  /// and could not be immediately acquired.
  bool try_lock() noexcept;

  /// Block the current thread until the lock is acquired.
  ///
  /// This will busy-wait until it acquires the lock.
  ///
  /// This has 'acquire' memory semantics and synchronises
  /// with prior calls to unlock().
  void lock() noexcept;

  /// Release the lock.
  ///
  /// This has 'release' memory semantics and synchronises with
  /// lock() and try_lock().
  void unlock() noexcept;

private:

  std::atomic<bool> m_isLocked;

};

RD_NAMESPACE_END

#endif //RENDU_SPIN_MUTEX_H
