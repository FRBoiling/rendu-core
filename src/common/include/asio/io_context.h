//
// Created by FRee2 on 2025/10/19.
//

#ifndef RENDUCORE_IO_CONTEXT_H
#define RENDUCORE_IO_CONTEXT_H

#include "asio.h"

class IoContext
{
public:
  using Executor = asio::io_context::executor_type;

  IoContext() : _impl() { }
  explicit IoContext(int concurrency_hint) : _impl(concurrency_hint) { }

  operator asio::io_context&() { return _impl; }
  operator asio::io_context const&() const { return _impl; }

  std::size_t run() { return _impl.run(); }
  std::size_t poll() { return _impl.poll(); }
  void stop() { _impl.stop(); }

  bool stopped() const { return _impl.stopped(); }
  void restart() { return _impl.restart(); }

  Executor get_executor() noexcept { return _impl.get_executor(); }

private:
  boost::asio::io_context _impl;
};

template<typename T>
inline decltype(auto) post(boost::asio::io_context& ioContext, T&& t)
{
  return boost::asio::post(ioContext, std::forward<T>(t));
}

template<typename T>
inline decltype(auto) post(boost::asio::io_context::executor_type& executor, T&& t)
{
  return boost::asio::post(executor, std::forward<T>(t));
}

using boost::asio::bind_executor;

template<typename T>
inline decltype(auto) get_io_context(T&& ioObject)
{
  return ioObject.get_executor().context();
}

#endif // RENDUCORE_IO_CONTEXT_H
