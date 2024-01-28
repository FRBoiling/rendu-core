/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_OUT_OF_RANGE_EXCEPTION_H
#define RENDU_OUT_OF_RANGE_EXCEPTION_H

#include "base_define.h"
#include <exception>

RD_NAMESPACE_BEGIN

class OutOfRangeException :std::exception{
};

RD_NAMESPACE_END

#endif//RENDU_OUT_OF_RANGE_EXCEPTION_H
