//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <common/log/file_sink.h>
#include <common/log/formatter.h>
#include <filesystem>
#include <fstream>

using namespace Rendu::log;

TEST_CASE("FileSink basic", "[log][file_sink]") {
    std::string filename = "/tmp/test_log.txt";

    // 清理之前的测试文件
    std::filesystem::remove(filename);

    {
        FileSink sink(filename, Level::Info);

        LogMessage msg;
        msg.level = Level::Info;
        msg.logger_name = "test";
        msg.message = "test message";
        msg.timestamp = "2026-01-22 12:00:00.000";
        msg.thread_id = std::this_thread::get_id();

        REQUIRE_NOTHROW(sink.log(msg));
    }

    // 检查文件是否创建并包含内容
    REQUIRE(std::filesystem::exists(filename));
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    REQUIRE(content.find("test message") != std::string::npos);

    // 清理
    std::filesystem::remove(filename);
}

TEST_CASE("FileSink set_level", "[log][file_sink]") {
    std::string filename = "/tmp/test_log_level.txt";
    std::filesystem::remove(filename);

    {
        FileSink sink(filename, Level::Info);
        REQUIRE(sink.level() == Level::Info);

        sink.set_level(Level::Debug);
        REQUIRE(sink.level() == Level::Debug);
    }

    std::filesystem::remove(filename);
}
