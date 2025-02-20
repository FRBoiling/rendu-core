/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_STATUS_H_
#define RENDU_BASIC_BASIC_BASE_STATUS_H_

#include "base_type_define.h"
#include "string_type_define.h"

RD_NAMESPACE_BEGIN
RD_DETAIL_NAMESPACE_BEGIN

/**
 * 说明：
 * 返回值为0，表示正常逻辑
 * 返回值为负整数，表示error逻辑，程序终止执行
 * 自定义返回值，请务必遵守以上约定
 */
static const int STATUS_OK = 0;                                 /** 正常流程返回值 */
static const int STATUS_ERR = -1;                               /** 异常流程返回值 */
static const int STATUS_CRASH = -996;                           /** 异常流程返回值 */

class Status {
public:
  explicit Status() = default;

  explicit Status(const std::string &errorInfo,
                   const std::string &locateInfo = RD_EMPTY) {
    this->error_code_ = STATUS_ERR;    // 默认的error code信息
    this->error_info_ = errorInfo;
    this->error_locate_ = locateInfo;
  }

  explicit Status(int errorCode, const std::string &errorInfo,
                   const std::string &locateInfo = RD_EMPTY) {
    this->error_code_ = errorCode;
    this->error_info_ = errorInfo;
    this->error_locate_ = locateInfo;
  }

  Status(const Status &status) {
    if (status.error_code_ == error_code_) {
      return;
    }

    this->error_code_ = status.error_code_;
    this->error_info_ = status.error_info_;
    this->error_locate_ = status.error_locate_;
  }

  Status(const Status &&status) noexcept {
    if (status.error_code_ == error_code_) {
      return;
    }

    this->error_code_ = status.error_code_;
    this->error_info_ = status.error_info_;
    this->error_locate_ = status.error_locate_;
  }

  Status& operator=(const Status& status) {
    if (this->error_code_ != status.error_code_) {
      // 如果status是正常的话，则所有数据保持不变
      this->error_code_ = status.error_code_;
      this->error_info_ = status.error_info_;
      this->error_locate_ = status.error_locate_;
    }
    return (*this);
  }

  Status& operator+=(const Status& cur) {
    /**
         * 如果当前状态已经异常，则不做改动
         * 如果当前状态正常，并且传入的状态是异常的话，则返回异常
         */
    if (!this->isErr() && cur.isErr()) {
      this->error_code_ = cur.error_code_;
      this->error_info_ = cur.error_info_;
      this->error_locate_ = cur.error_locate_;
    }

    return (*this);
  }

  /**
     * 恢复状态信息
     */
  void reset() {
    if (this->error_code_ != STATUS_OK) {
      this->error_code_ = STATUS_OK;
      this->error_info_.clear();
      this->error_locate_.clear();
    }
  }

  /**
     * 获取异常值信息
     * @return
     */
  int getCode() const {
    return this->error_code_;
  }

  /**
     * 获取异常信息
     * @return
     */
  const std::string& getInfo() const {
    return this->error_info_;
  }

  /**
     * 获取报错位置
     * @return
     */
  const std::string& getLocate() const {
    return this->error_locate_;
  }

  /**
     * 判断当前状态是否可行
     * @return
     */
  bool isOK() const {
    return STATUS_OK == error_code_;
  }

  /**
     * 判断当前状态是否可行
     * @return
     */
  bool isErr() const {
    return error_code_ < STATUS_OK;    // 约定异常信息，均为负值
  }

  /**
     * 判断当前状态是否是崩溃了
     * @return
     */
  bool isCrash() const {
    return STATUS_CRASH == error_code_;
  }

  /**
     * 设置异常信息
     * @param code
     * @param info
     * @return
     */
  Status* setInfo(int code, const std::string& info) {
    error_code_ = code;
    error_info_ = (STATUS_OK == error_code_) ? RD_EMPTY : info;
    return this;
  }

  /**
     * 设置异常信息
     * @param info
     * @return
     */
  Status* setErrorInfo(const std::string& info) {
    error_code_ = STATUS_ERR;
    error_info_ = info;
    return this;
  }

private:
  int error_code_ = STATUS_OK;                     // 错误码信息
  std::string error_info_;                         // 错误信息描述
  std::string error_locate_;                       // 错误发生的具体位置，形如：file|function|line
};

RD_DETAIL_NAMESPACE_END
RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_STATUS_H_
