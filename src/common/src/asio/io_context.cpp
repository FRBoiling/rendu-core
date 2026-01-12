#include "common/asio/io_context.h"

#include "common/asio/executor.h"
#include "common/asio/io_context_strand.h"
#include "common/asio/executor_work_guard.h"
#include "common/asio/signal_set.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        using namespace boost;

        // IoContext::Impl实现
        class IoContext::Impl
        {
        public:
            asio::io_context ioContext;

            Impl() = default;

            explicit Impl(int concurrencyHint) : ioContext(concurrencyHint)
            {
            }

            void post(std::function<void()> handler)
            {
                asio::post(ioContext, std::move(handler));
            }
        };

        // Executor::Impl实现
        class Executor::Impl
        {
        public:
            asio::io_context::executor_type executor;

            explicit Impl(asio::io_context::executor_type exec) : executor(std::move(exec))
            {
            }

            void post(const std::function<void()>& handler) const
            {
                asio::post(executor, handler);
            }
        };

        // Strand::Impl实现
        class IoContext::Strand::Impl
        {
        public:
            asio::strand<asio::io_context::executor_type> strand;

            explicit Impl(asio::io_context& ioContext) : strand(ioContext.get_executor())
            {
            }

            void post(const std::function<void()>& handler) const
            {
                asio::post(strand, handler);
            }

            void dispatch(const std::function<void()>& handler) const
            {
                asio::dispatch(strand, handler);
            }
        };

        // ExecutorWorkGuard::Impl实现
        class ExecutorWorkGuard::Impl
        {
        public:
            asio::executor_work_guard<asio::io_context::executor_type> workGuard;

            explicit Impl(const asio::io_context::executor_type& executor)
                : workGuard(executor)
            {
            }
        };

        // SignalSet::Impl实现
        class SignalSet::Impl
        {
        public:
            asio::signal_set signalSet;

            explicit Impl(asio::io_context& ioContext)
                : signalSet(ioContext)
            {
            }

            explicit Impl(asio::io_context& ioContext, int signalNumber)
                : signalSet(ioContext, signalNumber)
            {
            }

            explicit Impl(asio::io_context& ioContext, int signalNumber1, int signalNumber2)
                : signalSet(ioContext, signalNumber1, signalNumber2)
            {
            }

            explicit Impl(asio::io_context& ioContext, int signalNumber1, int signalNumber2, int signalNumber3)
                : signalSet(ioContext, signalNumber1, signalNumber2, signalNumber3)
            {
            }
        };

        // IoContext实现
        IoContext::IoContext() : m_pImpl(std::make_unique<Impl>())
        {
        }

        IoContext::IoContext(int concurrency_hint) : m_pImpl(std::make_unique<Impl>(concurrency_hint))
        {
        }

        IoContext::~IoContext() = default;
        IoContext::IoContext(IoContext&&) noexcept = default;
        IoContext& IoContext::operator=(IoContext&&) noexcept = default;

        std::size_t IoContext::run() const { return m_pImpl->ioContext.run(); }
        std::size_t IoContext::poll() const { return m_pImpl->ioContext.poll(); }
        void IoContext::stop() const { m_pImpl->ioContext.stop(); }
        bool IoContext::stopped() const { return m_pImpl->ioContext.stopped(); }
        void IoContext::restart() const { m_pImpl->ioContext.restart(); }

        void IoContext::post(const std::function<void()>& handler) const
        {
            m_pImpl->post(handler);
        }

        // Executor实现
        Executor::Executor() = default;
        Executor::~Executor() = default;
        Executor::Executor(Executor&&) noexcept = default;
        Executor& Executor::operator=(Executor&&) noexcept = default;

        Executor::Executor(std::unique_ptr<Impl> impl) : m_pImpl(std::move(impl))
        {
        }

        void Executor::post(const std::function<void()>& handler) const
        {
            m_pImpl->post(handler);
        }

        // Strand实现
        IoContext::Strand::Strand(std::unique_ptr<Impl> impl) : m_pImpl(std::move(impl))
        {
        }

        IoContext::Strand::Strand(const IoContext& ioContext)
        : m_pImpl(std::make_unique<Impl>(ioContext.m_pImpl->ioContext))
        {
        }

        IoContext::Strand::~Strand() = default;
        IoContext::Strand::Strand(Strand&&) noexcept = default;
        IoContext::Strand& IoContext::Strand::operator=(Strand&&) noexcept = default;

        void IoContext::Strand::post(const std::function<void()>& handler) const
        {
            m_pImpl->post(handler);
        }

        void IoContext::Strand::dispatch(const std::function<void()>& handler) const
        {
            m_pImpl->dispatch(handler);
        }

        // ExecutorWorkGuard实现
        ExecutorWorkGuard::ExecutorWorkGuard(const IoContext& io_context)
            : m_pImpl(std::make_unique<Impl>(io_context.m_pImpl->ioContext.get_executor()))
        {
        }

        ExecutorWorkGuard::~ExecutorWorkGuard() = default;
        ExecutorWorkGuard::ExecutorWorkGuard(ExecutorWorkGuard&&) noexcept = default;
        ExecutorWorkGuard& ExecutorWorkGuard::operator=(ExecutorWorkGuard&&) noexcept = default;

        // SignalSet实现
        SignalSet::SignalSet(const IoContext& ioContext)
            : m_pImpl(std::make_unique<Impl>(ioContext.m_pImpl->ioContext))
        {
        }

        SignalSet::SignalSet(const IoContext& ioContext, int signalNumber)
            : m_pImpl(std::make_unique<Impl>(ioContext.m_pImpl->ioContext, signalNumber))
        {
        }

        SignalSet::SignalSet(const IoContext& ioContext, int signalNumber1, int signalNumber2)
            : m_pImpl(std::make_unique<Impl>(ioContext.m_pImpl->ioContext, signalNumber1, signalNumber2))
        {
        }

        SignalSet::SignalSet(const IoContext& ioContext, int signalNumber1, int signalNumber2, int signalNumber3)
            : m_pImpl(
                std::make_unique<Impl>(ioContext.m_pImpl->ioContext, signalNumber1, signalNumber2, signalNumber3))
        {
        }

        SignalSet::~SignalSet() = default;
        SignalSet::SignalSet(SignalSet&&) noexcept = default;
        SignalSet& SignalSet::operator=(SignalSet&&) noexcept = default;

        void SignalSet::add(int signalNumber) const
        {
            m_pImpl->signalSet.add(signalNumber);
        }

        void SignalSet::remove(int signalNumber) const
        {
            m_pImpl->signalSet.remove(signalNumber);
        }

        void SignalSet::clear() const
        {
            m_pImpl->signalSet.clear();
        }

        void SignalSet::cancel() const
        {
            m_pImpl->signalSet.cancel();
        }

        void SignalSet::asyncWait(std::function<void(int)> handler) const
        {
            m_pImpl->signalSet.async_wait(
                [handler = std::move(handler)](const std::error_code& ec, int signal_number)
                {
                    if (!ec)
                    {
                        handler(signal_number);
                    }
                });
        }

        void SignalSet::asyncWait(std::function<void(const std::error_code&, int)> handler) const
        {
            m_pImpl->signalSet.async_wait(
                [handler = std::move(handler)](const std::error_code& ec, int signal_number)
                {
                    handler(std::error_code(ec.value(), ec.category()), signal_number);
                });
        }
    } // namespace Asio

END_NAMESPACE_COMMON
