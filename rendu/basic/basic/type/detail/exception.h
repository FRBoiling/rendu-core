/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_TYPE_EXCEPTION_H_
#define RENDU_BASIC_BASIC_BASE_TYPE_EXCEPTION_H_

#include "string_type_define.h"
#include <exception>

RD_NAMESPACE_BEGIN
RD_DETAIL_NAMESPACE_BEGIN

class Exception : public std::exception {
public:
  explicit Exception(const std::string& info,
                      const std::string& locate = RD_EMPTY) {
    /**
         * 这里的设计，和CStatus有一个联动
         * 如果不了解具体情况，不建议做任何修改
         */
    exception_info_ = locate + " | " + info;
  }

  /**
     * 获取异常信息
     * @return
     */
  const char* what() const noexcept override {
    return exception_info_.c_str();
  }

private:
  std::string exception_info_;            // 异常状态信息
};

RD_DETAIL_NAMESPACE_END
RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_TYPE_EXCEPTION_H_
