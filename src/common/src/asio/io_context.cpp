#include "common/asio/io_context.h"
#include "common/asio/executor.h"
#include "common/asio/io_context_strand.h"
#include "common/asio/executor_work_guard.h"
#include "common/asio/signal_set.h"
#include <asio/io_context.hpp>
#include <asio/io_context_strand.hpp>
#include <asio/post.hpp>
#include <asio/dispatch.hpp>
#include <asio/signal_set.hpp>
#include <asio/executor_work_guard.hpp>

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        // IoContext::Impl实现
        class IoContext::Impl
        {
        public:
            asio::io_context ioContext;

            Impl() = default;

            explicit Impl(int concurrency_hint) : ioContext(concurrency_hint)
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

            explicit Impl(asio::io_context::executor_type exec) : executor(exec)
            {
            }

            void post(std::function<void()> handler)
            {
                asio::post(executor, std::move(handler));
            }
        };

        // Strand::Impl实现
        class IoContext::Strand::Impl
        {
        public:
            asio::io_context::strand strand;

            explicit Impl(asio::io_context& ioContext) : strand(ioContext)
            {
            }

            void post(std::function<void()> handler)
            {
                asio::post(strand, std::move(handler));
            }

            void dispatch(std::function<void()> handler)
            {
                asio::dispatch(strand, std::move(handler));
            }
        };

        // ExecutorWorkGuard::Impl实现
        class ExecutorWorkGuard::Impl
        {
        public:
            asio::executor_work_guard<asio::io_context::executor_type> workGuard;

            explicit Impl(asio::io_context::executor_type executor)
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

            explicit Impl(asio::io_context& ioContext, int signal_number)
                : signalSet(ioContext, signal_number)
            {
            }

            Impl(asio::io_context& ioContext, int signal_number1, int signal_number2)
                : signalSet(ioContext, signal_number1, signal_number2)
            {
            }

            Impl(asio::io_context& ioContext, int signal_number1, int signal_number2, int signal_number3)
                : signalSet(ioContext, signal_number1, signal_number2, signal_number3)
            {
            }
        };

        // IoContext实现
        IoContext::IoContext() : pImpl_(std::make_unique<Impl>())
        {
        }

        IoContext::IoContext(int concurrency_hint) : pImpl_(std::make_unique<Impl>(concurrency_hint))
        {
        }

        IoContext::~IoContext() = default;
        IoContext::IoContext(IoContext&&) noexcept = default;
        IoContext& IoContext::operator=(IoContext&&) noexcept = default;

        std::size_t IoContext::run() { return pImpl_->ioContext.run(); }
        std::size_t IoContext::poll() { return pImpl_->ioContext.poll(); }
        void IoContext::stop() { pImpl_->ioContext.stop(); }
        bool IoContext::stopped() const { return pImpl_->ioContext.stopped(); }
        void IoContext::restart() { pImpl_->ioContext.restart(); }

        void IoContext::post(std::function<void()> handler) const
        {
            pImpl_->post(std::move(handler));
        }

        IoContext::Strand IoContext::make_strand()
        {
            return Strand(std::make_unique<Strand::Impl>(pImpl_->ioContext));
        }

        // Executor实现
        Executor::Executor() = default;
        Executor::~Executor() = default;
        Executor::Executor(Executor&&) noexcept = default;
        Executor& Executor::operator=(Executor&&) noexcept = default;

        Executor::Executor(std::unique_ptr<Impl> impl) : pImpl_(std::move(impl))
        {
        }

        void Executor::post(std::function<void()> handler)
        {
            pImpl_->post(std::move(handler));
        }

        // Strand实现
        IoContext::Strand::Strand(std::unique_ptr<Impl> impl) : pImpl_(std::move(impl))
        {
        }

        IoContext::Strand::~Strand() = default;
        IoContext::Strand::Strand(IoContext::Strand&&) noexcept = default;
        IoContext::Strand& IoContext::Strand::operator=(IoContext::Strand&&) noexcept = default;

        void IoContext::Strand::post(std::function<void()> handler) const
        {
            pImpl_->post(std::move(handler));
        }

        void IoContext::Strand::dispatch(std::function<void()> handler) const
        {
            pImpl_->dispatch(std::move(handler));
        }

        // ExecutorWorkGuard实现
        ExecutorWorkGuard::ExecutorWorkGuard(IoContext& io_context)
            : pImpl_(std::make_unique<Impl>(io_context.pImpl_->ioContext.get_executor()))
        {
        }

        ExecutorWorkGuard::~ExecutorWorkGuard() = default;
        ExecutorWorkGuard::ExecutorWorkGuard(ExecutorWorkGuard&&) noexcept = default;
        ExecutorWorkGuard& ExecutorWorkGuard::operator=(ExecutorWorkGuard&&) noexcept = default;

        // SignalSet实现
        SignalSet::SignalSet(IoContext& ioContext)
            : pImpl_(std::make_unique<Impl>(ioContext.pImpl_->ioContext))
        {
        }

        SignalSet::SignalSet(IoContext& ioContext, int signal_number)
            : pImpl_(std::make_unique<Impl>(ioContext.pImpl_->ioContext, signal_number))
        {
        }

        SignalSet::SignalSet(IoContext& ioContext, int signal_number1, int signal_number2)
            : pImpl_(std::make_unique<Impl>(ioContext.pImpl_->ioContext, signal_number1, signal_number2))
        {
        }

        SignalSet::SignalSet(IoContext& ioContext, int signal_number1, int signal_number2, int signal_number3)
            : pImpl_(
                std::make_unique<Impl>(ioContext.pImpl_->ioContext, signal_number1, signal_number2, signal_number3))
        {
        }

        SignalSet::~SignalSet() = default;
        SignalSet::SignalSet(SignalSet&&) noexcept = default;
        SignalSet& SignalSet::operator=(SignalSet&&) noexcept = default;

        void SignalSet::add(int signal_number)
        {
            pImpl_->signalSet.add(signal_number);
        }

        void SignalSet::remove(int signal_number)
        {
            pImpl_->signalSet.remove(signal_number);
        }

        void SignalSet::clear()
        {
            pImpl_->signalSet.clear();
        }

        void SignalSet::cancel()
        {
            pImpl_->signalSet.cancel();
        }

        void SignalSet::async_wait(std::function<void(int)> handler)
        {
            pImpl_->signalSet.async_wait(
                [handler = std::move(handler)](const asio::error_code& ec, int signal_number)
                {
                    if (!ec)
                    {
                        handler(signal_number);
                    }
                });
        }

        void SignalSet::async_wait(std::function<void(const std::error_code&, int)> handler)
        {
            pImpl_->signalSet.async_wait(
                [handler = std::move(handler)](const asio::error_code& ec, int signal_number)
                {
                    handler(ec, signal_number);
                });
        }
    } // namespace Asio

END_NAMESPACE_COMMON
