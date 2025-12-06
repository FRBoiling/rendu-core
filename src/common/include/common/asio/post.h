//
// Created by 沸腾 on 2025/11/14.
//

#ifndef RENDU_POST_H
#define RENDU_POST_H

#include "common/define.h"
#include "io_context.h"
#include "io_context_strand.h"

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        template <typename T>
        void Post(IoContext& ioContext, T&& handler)
        {
            ioContext.post(std::forward<T>(handler));
        }

        template <typename T>
        void Post(IoContext::Strand& strand, T&& handler)
        {
            auto handler_ptr = std::make_shared<std::decay_t<T>>(std::forward<T>(handler));
            strand.post([handler_ptr]()
            {
                (*handler_ptr)();
            });
        }
    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_POST_H
