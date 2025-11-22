//
// Created by 沸腾 on 2025/11/14.
//

#ifndef RENDU_STRAND_H
#define RENDU_STRAND_H

#include <asio/io_context_strand.hpp>

#include "io_context.h"
#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        /**
          Hack to make it possible to forward declare strand (which is a inner class)
        */
        class Strand : public asio::io_context::strand
        {
        public:
            Strand(IoContext& ioContext) : asio::io_context::strand(ioContext)
            {
            }
        };

        template <typename T>
        inline decltype(auto) post(asio::io_context::strand& strand, T&& t)
        {
            return asio::post(strand, std::forward<T>(t));
        }
    } // namespace Asio

END_NAMESPACE_COMMON


#endif //RENDU_STRAND_H
