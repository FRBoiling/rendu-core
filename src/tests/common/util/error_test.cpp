//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <common/util/error.h>
#include <common/define.h>
#include <variant>
#include <type_traits>

using namespace Rendu;

TEST_CASE("Error基本功能", "[util][error]") {
    Error err(ErrorCode::Success, "Operation successful");
    REQUIRE(err.code() == ErrorCode::Success);
    REQUIRE(err.message() == "Operation successful");
    REQUIRE(err.is_success() == true);
}

TEST_CASE("Error is_success 判断", "[util][error]") {
    Error success(ErrorCode::Success, "Success");
    REQUIRE(success.is_success() == true);

    Error failure(ErrorCode::InvalidArgument, "Invalid argument");
    REQUIRE(failure.is_success() == false);

    Error io_error(ErrorCode::IOError, "I/O error");
    REQUIRE(io_error.is_success() == false);
}

TEST_CASE("Error 消息", "[util][error]") {
    Error err(ErrorCode::NotFound, "Resource not found");
    REQUIRE(err.message() == "Resource not found");

    Error empty(ErrorCode::Timeout, "");
    REQUIRE(empty.message() == "");
}

TEST_CASE("Error 错误码", "[util][error]") {
    REQUIRE(Error(ErrorCode::Success).code() == ErrorCode::Success);
    REQUIRE(Error(ErrorCode::InvalidArgument).code() == ErrorCode::InvalidArgument);
    REQUIRE(Error(ErrorCode::NotFound).code() == ErrorCode::NotFound);
    REQUIRE(Error(ErrorCode::AlreadyExists).code() == ErrorCode::AlreadyExists);
    REQUIRE(Error(ErrorCode::Timeout).code() == ErrorCode::Timeout);
    REQUIRE(Error(ErrorCode::IOError).code() == ErrorCode::IOError);
    REQUIRE(Error(ErrorCode::NetworkError).code() == ErrorCode::NetworkError);
    REQUIRE(Error(ErrorCode::SerializationError).code() == ErrorCode::SerializationError);
    REQUIRE(Error(ErrorCode::Unknown).code() == ErrorCode::Unknown);
}

TEST_CASE("Result：成功值", "[util][error]") {
    Result<int> result = 42;

    REQUIRE(std::holds_alternative<int>(result) == true);
    REQUIRE(std::holds_alternative<Error>(result) == false);
    REQUIRE(std::get<int>(result) == 42);
}

TEST_CASE("Result：错误", "[util][error]") {
    Result<int> result = Error(ErrorCode::InvalidArgument, "Bad argument");

    REQUIRE(std::holds_alternative<Error>(result) == true);
    REQUIRE(std::holds_alternative<int>(result) == false);
    REQUIRE(std::get<Error>(result).code() == ErrorCode::InvalidArgument);
    REQUIRE(std::get<Error>(result).message() == "Bad argument");
}

TEST_CASE("Result：字符串类型", "[util][error]") {
    Result<std::string> result = std::string("hello");

    REQUIRE(std::holds_alternative<std::string>(result) == true);
    REQUIRE(std::get<std::string>(result) == "hello");

    Result<std::string> error_result = Error(ErrorCode::IOError, "Failed");
    REQUIRE(std::holds_alternative<Error>(error_result) == true);
    REQUIRE(std::get<Error>(error_result).code() == ErrorCode::IOError);
}

TEST_CASE("Error 默认构造", "[util][error]") {
    Error err;
    REQUIRE(err.code() == ErrorCode::Success);
    REQUIRE(err.message() == "");
    REQUIRE(err.is_success() == true);
}
