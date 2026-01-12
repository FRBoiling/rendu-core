//
// Created by 沸腾 on 2025/11/14.
//

#ifndef RENDU_IO_CONTEXT_STRAND_H
#define RENDU_IO_CONTEXT_STRAND_H

#include <memory>
#include <functional>
#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        class IoContext::Strand
        {
        public:
            Strand(const IoContext& ioContext);
            ~Strand();

            Strand(const Strand&) = delete;
            Strand& operator=(const Strand&) = delete;
            Strand(Strand&&) noexcept;
            Strand& operator=(Strand&&) noexcept;

            void post(const std::function<void()>& handler) const;
            void dispatch(const std::function<void()>& handler) const;

        private:
            friend class IoContext;
            class Impl;
            std::unique_ptr<Impl> m_pImpl;

            explicit Strand(std::unique_ptr<Impl> impl);
        };


    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_IO_CONTEXT_STRAND_H