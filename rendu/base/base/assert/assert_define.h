/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_BASE_ASSERT_DEFINE_H
#define RENDU_BASE_ASSERT_DEFINE_H

#include "base_define.h"

#ifdef _WIN32
#include <crtdbg.h> // Windows-specific header for _ASSERT
#else

#include <cassert> // Standard header for assert

#endif


RD_NAMESPACE_BEGIN

#ifdef _WIN32
#define ASSERT(expr) _ASSERT(expr)
#else
#define ASSERT(expr) assert(expr)
#endif

RD_NAMESPACE_END

#endif//RENDU_BASE_ASSERT_DEFINE_H
