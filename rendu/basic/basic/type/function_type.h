/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_BASIC_BASIC_TYPE_FUNCTION_TYPE_H_
#define RENDU_BASIC_BASIC_TYPE_FUNCTION_TYPE_H_

#include "detail/function_type_define.h"
#include "value_type.h"

RD_NAMESPACE_BEGIN

/**
 * 描述函数类型
 */
enum class EFunctionType {
  INIT = 1,              /** 初始化函数 */
  RUN = 2,               /** 执行函数 */
  DESTROY = 3            /** 释放函数 */
};

using RD_DEFAULT_FUNCTION = rendu::detail::RD_DEFAULT_FUNCTION;
using RD_DEFAULT_CONST_FUNCTION_REF = rendu::detail::RD_DEFAULT_CONST_FUNCTION_REF;

using RD_STATUS_FUNCTION = rendu::detail::RD_STATUS_FUNCTION;
using RD_STATUS_CONST_FUNCTION_REF = rendu::detail::RD_STATUS_CONST_FUNCTION_REF;

using RD_CALLBACK_FUNCTION = rendu::detail::RD_CALLBACK_FUNCTION;
using RD_CALLBACK_CONST_FUNCTION_REF = rendu::detail::RD_CALLBACK_CONST_FUNCTION_REF;

/** 开启函数流程 */
#define RD_FUNCTION_BEGIN                                           \
    Status status;                                                     \

/** 结束函数流程 */
#define RD_FUNCTION_END                                             \
    return status;                                                      \

/** 无任何功能函数 */
#define RD_EMPTY_FUNCTION                                           \
    return Status();                                                   \


/** 获取当前代码所在的位置信息 */
#define RD_GET_LOCATE                                               \
    (std::string(__FILE__) + " | " + std::string(__FUNCTION__)          \
    + " | line = [" + ::std::to_string( __LINE__) + "]")


/** 生成一个包含异常位置的 Status
 * 这里这样实现，是为了符合 Status 类似写法
 * */
#define ErrStatus(info)                                                \
    Status(info, RD_GET_LOCATE)                                    \

/** 返回异常信息和状态 */
#define RD_RETURN_ERROR_STATUS(info)                                \
    return ErrStatus(info);                                            \

/** 根据条件判断是否返回错误状态 */
#define RD_RETURN_ERROR_STATUS_BY_CONDITION(cond, info)             \
    if (unlikely(cond)) { RD_RETURN_ERROR_STATUS(info); }           \

/** 不支持当前功能 */
#define RD_NO_SUPPORT                                               \
    return ErrStatus(RD_FUNCTION_NO_SUPPORT);                      \



/** 抛出异常 */
#define RD_THROW_EXCEPTION(info)                                    \
    throw Exception(info, RD_GET_LOCATE);                          \

/** 在异常状态的情况下，抛出异常 */
#define RD_THROW_EXCEPTION_BY_STATUS(status)                        \
    if (unlikely((status).isErr())) {                                   \
        RD_THROW_EXCEPTION((status).getInfo()); }                   \

/** 根据条件判断是否抛出异常 */
#define RD_THROW_EXCEPTION_BY_CONDITION(cond, info)                 \
    if (unlikely(cond)) { RD_THROW_EXCEPTION(info); }               \

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_TYPE_FUNCTION_TYPE_H_
