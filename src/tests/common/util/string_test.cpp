#include <catch2/catch_test_macros.hpp>
#include <common/util/string.h>
#include <common/define.h>

using namespace Rendu::str;

TEST_CASE("string trim", "[util][string]") {
    REQUIRE(trim("  hello  ") == "hello");
    REQUIRE(trim("\thello\t") == "hello");
    REQUIRE(trim("\nhello\n") == "hello");
    REQUIRE(trim("hello") == "hello");
    REQUIRE(trim("") == "");
    REQUIRE(trim("   ") == "");
}

TEST_CASE("string split", "[util][string]") {
    auto parts = split("a,b,c", ',');
    REQUIRE(parts.size() == 3);
    REQUIRE(parts[0] == "a");
    REQUIRE(parts[1] == "b");
    REQUIRE(parts[2] == "c");

    parts = split("a,b,c,", ',');
    REQUIRE(parts.size() == 4);
    REQUIRE(parts[3] == "");

    parts = split("single", ',');
    REQUIRE(parts.size() == 1);
    REQUIRE(parts[0] == "single");
}

TEST_CASE("string join", "[util][string]") {
    std::vector<std::string> parts = {"a", "b", "c"};
    REQUIRE(join(parts, ",") == "a,b,c");
    REQUIRE(join(parts, "-") == "a-b-c");

    REQUIRE(join({}, ",") == "");

    std::vector<std::string> single = {"single"};
    REQUIRE(join(single, ",") == "single");
}

TEST_CASE("string to_lower", "[util][string]") {
    REQUIRE(to_lower("HELLO") == "hello");
    REQUIRE(to_lower("Hello") == "hello");
    REQUIRE(to_lower("hello") == "hello");
    REQUIRE(to_lower("") == "");
    REQUIRE(to_lower("Hello World") == "hello world");
}

TEST_CASE("string to_upper", "[util][string]") {
    REQUIRE(to_upper("hello") == "HELLO");
    REQUIRE(to_upper("Hello") == "HELLO");
    REQUIRE(to_upper("HELLO") == "HELLO");
    REQUIRE(to_upper("") == "");
    REQUIRE(to_upper("Hello World") == "HELLO WORLD");
}

TEST_CASE("string starts_with", "[util][string]") {
    REQUIRE(starts_with("hello world", "hello") == true);
    REQUIRE(starts_with("hello world", "he") == true);
    REQUIRE(starts_with("hello world", "") == true);
    REQUIRE(starts_with("hello", "hello world") == false);
    REQUIRE(starts_with("", "hello") == false);
    REQUIRE(starts_with("", "") == true);
}

TEST_CASE("string ends_with", "[util][string]") {
    REQUIRE(ends_with("hello world", "world") == true);
    REQUIRE(ends_with("hello world", "ld") == true);
    REQUIRE(ends_with("hello world", "") == true);
    REQUIRE(ends_with("hello", "hello world") == false);
    REQUIRE(ends_with("", "hello") == false);
    REQUIRE(ends_with("", "") == true);
}

TEST_CASE("string contains", "[util][string]") {
    REQUIRE(contains("hello world", "world") == true);
    REQUIRE(contains("hello world", "hello") == true);
    REQUIRE(contains("hello world", "lo wo") == true);
    REQUIRE(contains("hello world", "xyz") == false);
    REQUIRE(contains("", "") == true);
    REQUIRE(contains("hello", "") == true);
}

TEST_CASE("string replace", "[util][string]") {
    REQUIRE(replace("hello world", "world", "there") == "hello there");
    REQUIRE(replace("aaa", "a", "b") == "bbb");
    REQUIRE(replace("hello world hello", "hello", "hi") == "hi world hi");
    REQUIRE(replace("no match", "xyz", "abc") == "no match");
    REQUIRE(replace("test", "", "a") == "test");
    REQUIRE(replace("", "a", "b") == "");
}
