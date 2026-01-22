#include <catch2/catch_test_macros.hpp>
#include <common/util/container.h>
#include <common/define.h>
#include <map>
#include <unordered_map>
#include <list>

using namespace Rendu::container;

TEST_CASE("contains with vector", "[util][container]") {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    REQUIRE(contains(vec, 1) == true);
    REQUIRE(contains(vec, 3) == true);
    REQUIRE(contains(vec, 5) == true);
    REQUIRE(contains(vec, 6) == false);
    REQUIRE(contains(vec, 0) == false);
}

TEST_CASE("contains with list", "[util][container]") {
    std::list<int> list = {10, 20, 30};

    REQUIRE(contains(list, 10) == true);
    REQUIRE(contains(list, 20) == true);
    REQUIRE(contains(list, 30) == true);
    REQUIRE(contains(list, 40) == false);
}

TEST_CASE("contains with set", "[util][container]") {
    std::set<int> set = {1, 2, 3};

    REQUIRE(contains(set, 1) == true);
    REQUIRE(contains(set, 2) == true);
    REQUIRE(contains(set, 3) == true);
    REQUIRE(contains(set, 4) == false);
}

TEST_CASE("contains with unordered_set", "[util][container]") {
    std::unordered_set<int> uset = {100, 200, 300};

    REQUIRE(contains(uset, 100) == true);
    REQUIRE(contains(uset, 200) == true);
    REQUIRE(contains(uset, 300) == true);
    REQUIRE(contains(uset, 400) == false);
}

TEST_CASE("contains with empty container", "[util][container]") {
    std::vector<int> empty;
    REQUIRE(contains(empty, 1) == false);
}

TEST_CASE("erase_if with vector", "[util][container]") {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};

    erase_if(vec, [](int x) { return x % 2 == 0; });
    REQUIRE(vec.size() == 3);
    REQUIRE(vec == std::vector<int>{1, 3, 5});
}

TEST_CASE("erase_if with list", "[util][container]") {
    std::list<int> list = {1, 2, 3, 4, 5};

    erase_if(list, [](int x) { return x > 3; });
    REQUIRE(list.size() == 3);
    REQUIRE(list == std::list<int>{1, 2, 3});
}

TEST_CASE("erase_if all elements", "[util][container]") {
    std::vector<int> vec = {1, 2, 3};

    erase_if(vec, [](int x) { return true; });
    REQUIRE(vec.empty());
}

TEST_CASE("erase_if no elements", "[util][container]") {
    std::vector<int> vec = {1, 2, 3};

    erase_if(vec, [](int x) { return false; });
    REQUIRE(vec.size() == 3);
    REQUIRE(vec == std::vector<int>{1, 2, 3});
}

TEST_CASE("find_or with map", "[util][container]") {
    std::map<std::string, int> map = {{"a", 1}, {"b", 2}, {"c", 3}};

    REQUIRE(find_or<std::map<std::string, int>, std::string, int>(map, "a", 0) == 1);
    REQUIRE(find_or<std::map<std::string, int>, std::string, int>(map, "b", 0) == 2);
    REQUIRE(find_or<std::map<std::string, int>, std::string, int>(map, "c", 0) == 3);
    REQUIRE(find_or<std::map<std::string, int>, std::string, int>(map, "d", 99) == 99);
}

TEST_CASE("find_or with unordered_map", "[util][container]") {
    std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}};

    REQUIRE(find_or<std::unordered_map<int, std::string>, int, std::string>(umap, 1, "default") == "one");
    REQUIRE(find_or<std::unordered_map<int, std::string>, int, std::string>(umap, 2, "default") == "two");
    REQUIRE(find_or<std::unordered_map<int, std::string>, int, std::string>(umap, 3, "default") == "default");
}

TEST_CASE("map_keys with map", "[util][container]") {
    std::map<int, std::string> map = {{1, "a"}, {2, "b"}, {3, "c"}};

    auto keys = map_keys(map);
    REQUIRE(keys.size() == 3);
    REQUIRE(keys == std::vector<int>{1, 2, 3});
}

TEST_CASE("map_keys with unordered_map", "[util][container]") {
    std::unordered_map<int, std::string> umap = {{10, "x"}, {20, "y"}, {30, "z"}};

    auto keys = map_keys(umap);
    REQUIRE(keys.size() == 3);
    std::sort(keys.begin(), keys.end());
    REQUIRE(keys == std::vector<int>{10, 20, 30});
}

TEST_CASE("map_values with map", "[util][container]") {
    std::map<int, std::string> map = {{1, "a"}, {2, "b"}, {3, "c"}};

    auto values = map_values(map);
    REQUIRE(values.size() == 3);
    REQUIRE(values == std::vector<std::string>{"a", "b", "c"});
}

TEST_CASE("map_values with unordered_map", "[util][container]") {
    std::unordered_map<int, std::string> umap = {{10, "x"}, {20, "y"}, {30, "z"}};

    auto values = map_values(umap);
    REQUIRE(values.size() == 3);
    std::sort(values.begin(), values.end());
    REQUIRE(values == std::vector<std::string>{"x", "y", "z"});
}

TEST_CASE("find_or with default value type", "[util][container]") {
    std::map<std::string, std::string> map = {{"key", "value"}};

    auto result = find_or<std::map<std::string, std::string>, std::string, std::string>(map, "key", "default");
    REQUIRE(result == "value");

    auto missing = find_or<std::map<std::string, std::string>, std::string, std::string>(map, "missing", "default");
    REQUIRE(missing == "default");
}
