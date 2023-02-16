/*
* Created by boil on 2023/2/11.
*/

#ifndef RENDU_CORE_CORE_DEFINE_DEFINE_H_
#define RENDU_CORE_CORE_DEFINE_DEFINE_H_

#ifndef RD_ID_TYPE
#    include <cstdint>
#    define RD_ID_TYPE std::uint32_t
#endif

#ifdef RD_USE_ATOMIC
#    include <atomic>
#    define RD_MAYBE_ATOMIC(Type) std::atomic<Type>
#else
#    define RD_MAYBE_ATOMIC(Type) Type
#endif

#ifndef RD_PACKED_PAGE
#    define RD_PACKED_PAGE 1024
#endif
#ifndef RD_SPARSE_PAGE
#    define RD_SPARSE_PAGE 4096
#endif

#   define RD_ETO_TYPE(Type) Type

#endif //RENDU_CORE_CORE_DEFINE_DEFINE_H_
