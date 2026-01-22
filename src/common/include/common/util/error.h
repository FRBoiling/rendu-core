#pragma once

#include "common/define.h"
#include <string>
#include <string_view>
#include <optional>
#include <variant>

BEGIN_NAMESPACE_COMMON


/**
 * @brief 错误码枚举
 */
enum class ErrorCode {
    Success = 0,
    InvalidArgument,
    NotFound,
    AlreadyExists,
    Timeout,
    IOError,
    NetworkError,
    SerializationError,
    Unknown
};

/**
 * @brief 错误信息类
 */
class Error {
public:
    /**
     * @brief 默认构造函数
     */
    Error() : code_(ErrorCode::Success), message_("") {}

    /**
     * @brief 构造函数
     * @param code 错误码
     * @param message 错误信息
     */
    Error(ErrorCode code, std::string_view message = "")
        : code_(code), message_(message) {}

    /**
     * @brief 获取错误码
     * @return 错误码
     */
    ErrorCode code() const { return code_; }

    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    const std::string& message() const { return message_; }

    /**
     * @brief 检查是否成功
     * @return 是否成功
     */
    bool is_success() const { return code_ == ErrorCode::Success; }

private:
    ErrorCode code_;
    std::string message_;
};

/**
 * @brief Result 类型，用于返回值或错误
 * @tparam T 返回值类型
 */
template<typename T>
using Result = std::variant<T, Error>;

/**
 * @brief 抛出错误宏
 */
#define RENDU_THROW(code, msg) throw Rendu::Error(code, msg)

/**
 * @brief 错误处理宏
 */
#define RENDU_TRY(expr) \
    do { \
        auto _rendu_result = (expr); \
        if (std::holds_alternative<Rendu::Error>(_rendu_result)) { \
            return std::get<Rendu::Error>(_rendu_result); \
        } \
    } while(0)

END_NAMESPACE_COMMON
