/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_OBJECT_H_
#define RENDU_BASIC_BASIC_BASE_OBJECT_H_

#include "function_type.h"
#include "value_type.h"

RD_NAMESPACE_BEGIN

class Object {
  public:
    /**
     * 默认构造函数
     */
    explicit Object() = default;
    /**
     * 默认析构函数
     */
    virtual ~Object() = default;
  public:
    /**
     * 初始化函数
     */
    virtual Status init() {
      RD_EMPTY_FUNCTION
    }

    /**
     * 流程处理函数
     */
    virtual Status run() = 0;

    /**
     * 释放函数
     */
    virtual Status destroy() {
      RD_EMPTY_FUNCTION
    }


  };

  RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_OBJECT_H_
