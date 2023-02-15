/*
* Created by boil on 2023/2/16.
*/

#ifndef RENDU_CORE_CORE_DEFINE_H_
#define RENDU_CORE_CORE_DEFINE_H_

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

#endif //RENDU_CORE_CORE_DEFINE_H_
