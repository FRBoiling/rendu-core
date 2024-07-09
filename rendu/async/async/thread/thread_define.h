/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_THREAD_THREAD_DEFINE_H_
#define RENDU_ASYNC_ASYNC_THREAD_THREAD_DEFINE_H_

#include "async_define.h"

RD_ASYNC_NAMESPACE_BEGIN

#define RD_SLEEP_SECOND(s)                                                  \
    std::this_thread::sleep_for(std::chrono::seconds(s));                       \

#define RD_SLEEP_MILLISECOND(ms)                                            \
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));                 \

RD_ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_THREAD_THREAD_DEFINE_H_
