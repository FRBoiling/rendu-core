//**********************************
//  Created by boil on 2026/1/25.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <variant>
#include "common/ser/json_ser.h"
#include "common/define.h"

using namespace Rendu::ser;

TEST_CASE("JsonSerializer 基本序列化", "[json][serialize]") {
    SECTION("整数序列化") {
        int value = 42;
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "42");
    }

    SECTION("int64_t 序列化") {
        int64_t value = 9007199254740992LL;
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "9007199254740992");
    }

    SECTION("浮点数序列化") {
        double value = 3.14159;
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str.find("3.14159") == 0);
    }

    SECTION("字符串序列化") {
        std::string value = "hello world";
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "\"hello world\"");
    }

    SECTION("布尔值序列化") {
        bool value = true;
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "true");

        value = false;
        result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        buffer = std::get<ByteBuffer>(result);
        str = std::string(buffer.begin(), buffer.end());
        REQUIRE(str == "false");
    }

    SECTION("空值序列化") {
        auto result = JsonSerializer::serialize_json_null();
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "null");
    }
}

TEST_CASE("JsonSerializer 反序列化", "[json][deserialize]") {
    SECTION("整数反序列化") {
        ByteBuffer buffer{'4', '2'};
        auto result = JsonSerializer::deserialize_json_int64(buffer);
        REQUIRE(std::holds_alternative<int64_t>(result));
        REQUIRE(std::get<int64_t>(result) == 42);
    }

    SECTION("浮点数反序列化") {
        ByteBuffer buffer{'3', '.', '1', '4', '1', '5', '9'};
        auto result = JsonSerializer::deserialize_json_double(buffer);
        REQUIRE(std::holds_alternative<double>(result));
        REQUIRE(std::get<double>(result) == Catch::Approx(3.14159));
    }

    SECTION("字符串反序列化") {
        ByteBuffer buffer{'"', 'h', 'e', 'l', 'l', 'o', '"'};
        auto result = JsonSerializer::deserialize_json_string(buffer);
        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "hello");
    }

    SECTION("布尔值反序列化") {
        ByteBuffer buffer{'t', 'r', 'u', 'e'};
        auto result = JsonSerializer::deserialize_json_bool(buffer);
        REQUIRE(std::holds_alternative<bool>(result));
        REQUIRE(std::get<bool>(result) == true);

        ByteBuffer buffer2{'f', 'a', 'l', 's', 'e'};
        result = JsonSerializer::deserialize_json_bool(buffer2);
        REQUIRE(std::holds_alternative<bool>(result));
        REQUIRE(std::get<bool>(result) == false);
    }
}

TEST_CASE("JsonSerializer 数组", "[json][array]") {
    SECTION("整数数组") {
        std::vector<int> vec{1, 2, 3, 4, 5};
        auto result = JsonSerializer::serialize_json_array(vec);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "[1,2,3,4,5]");

        auto deserialized = JsonSerializer::deserialize_json_array<int64_t>(buffer);
        REQUIRE(std::holds_alternative<std::vector<int64_t>>(deserialized));
        auto deserialized_vec = std::get<std::vector<int64_t>>(deserialized);
        REQUIRE(deserialized_vec.size() == 5);
        REQUIRE(deserialized_vec[0] == 1);
        REQUIRE(deserialized_vec[4] == 5);
    }

    SECTION("字符串数组") {
        std::vector<std::string> vec{"hello", "world", "json"};
        auto result = JsonSerializer::serialize_json_array(vec);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);

        auto deserialized = JsonSerializer::deserialize_json_array<std::string>(buffer);
        REQUIRE(std::holds_alternative<std::vector<std::string>>(deserialized));
        auto deserialized_vec = std::get<std::vector<std::string>>(deserialized);
        REQUIRE(deserialized_vec.size() == 3);
        REQUIRE(deserialized_vec[0] == "hello");
        REQUIRE(deserialized_vec[2] == "json");
    }

    SECTION("空数组") {
        std::vector<int> vec;
        auto result = JsonSerializer::serialize_json_array(vec);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "[]");
    }
}

TEST_CASE("JsonSerializer 特殊字符", "[json][special]") {
    SECTION("Unicode 字符") {
        std::string value = "你好，世界";
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);

        auto deserialized = JsonSerializer::deserialize_json_string(buffer);
        REQUIRE(std::holds_alternative<std::string>(deserialized));
        REQUIRE(std::get<std::string>(deserialized) == value);
    }

    SECTION("转义字符") {
        std::string value = "line1\nline2\ttab";
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);

        auto deserialized = JsonSerializer::deserialize_json_string(buffer);
        REQUIRE(std::holds_alternative<std::string>(deserialized));
        REQUIRE(std::get<std::string>(deserialized) == value);
    }

    SECTION("引号和反斜杠") {
        std::string value = "quote: \" and backslash: \\";
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);

        auto deserialized = JsonSerializer::deserialize_json_string(buffer);
        REQUIRE(std::holds_alternative<std::string>(deserialized));
        REQUIRE(std::get<std::string>(deserialized) == value);
    }
}

TEST_CASE("JsonSerializer 错误处理", "[json][error]") {
    SECTION("无效的 JSON") {
        ByteBuffer buffer{'{', 'i', 'n', 'v', 'a', 'l', 'i', 'd', '}'};
        auto result = JsonSerializer::deserialize_json_int64(buffer);
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("空数据") {
        ByteBuffer buffer;
        auto result = JsonSerializer::deserialize_json_int64(buffer);
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("不完整的 JSON") {
        ByteBuffer buffer{'{', '"', 'k', 'e', 'y', '"', ':'};
        auto result = JsonSerializer::deserialize_json_int64(buffer);
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("类型不匹配") {
        ByteBuffer buffer{'"', '4', '2', '"'};
        auto result = JsonSerializer::deserialize_json_int64(buffer);
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }
}

TEST_CASE("JsonSerializer 大数据量", "[json][large]") {
    SECTION("大数组") {
        std::vector<int> vec(1000);
        for (int i = 0; i < 1000; ++i) {
            vec[i] = i;
        }
        
        auto result = JsonSerializer::serialize_json_array(vec);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);

        auto deserialized = JsonSerializer::deserialize_json_array<int64_t>(buffer);
        REQUIRE(std::holds_alternative<std::vector<int64_t>>(deserialized));
        auto deserialized_vec = std::get<std::vector<int64_t>>(deserialized);
        REQUIRE(deserialized_vec.size() == 1000);
        REQUIRE(deserialized_vec[0] == 0);
        REQUIRE(deserialized_vec[999] == 999);
    }

    SECTION("长字符串") {
        std::string str(10000, 'a');
        auto result = JsonSerializer::serialize_json(str);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);

        auto deserialized = JsonSerializer::deserialize_json_string(buffer);
        REQUIRE(std::holds_alternative<std::string>(deserialized));
        REQUIRE(std::get<std::string>(deserialized).size() == 10000);
    }
}

TEST_CASE("JsonSerializer 边界值", "[json][boundary]") {
    SECTION("零值") {
        auto result1 = JsonSerializer::serialize_json(0);
        REQUIRE(std::holds_alternative<ByteBuffer>(result1));
        auto result2 = JsonSerializer::serialize_json(int64_t{0});
        REQUIRE(std::holds_alternative<ByteBuffer>(result2));
        auto result3 = JsonSerializer::serialize_json(0.0);
        REQUIRE(std::holds_alternative<ByteBuffer>(result3));
        auto result4 = JsonSerializer::serialize_json(false);
        REQUIRE(std::holds_alternative<ByteBuffer>(result4));
    }

    SECTION("负数") {
        int value = -42;
        auto result = JsonSerializer::serialize_json(value);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto buffer = std::get<ByteBuffer>(result);
        std::string str(buffer.begin(), buffer.end());
        REQUIRE(str == "-42");
    }

    SECTION("极大值") {
        auto result1 = JsonSerializer::serialize_json(int64_t{INT64_MAX});
        REQUIRE(std::holds_alternative<ByteBuffer>(result1));
        auto result2 = JsonSerializer::serialize_json(int64_t{INT64_MIN});
        REQUIRE(std::holds_alternative<ByteBuffer>(result2));
    }
}

TEST_CASE("JsonSerializer is_valid_json", "[json][validate]") {
    SECTION("有效的 JSON") {
        ByteBuffer buffer{'"', 'h', 'e', 'l', 'l', 'o', '"'};
        REQUIRE(JsonSerializer::is_valid_json(buffer));

        ByteBuffer buffer2{'1', '2', '3'};
        REQUIRE(JsonSerializer::is_valid_json(buffer2));

        std::string str = R"({"key":"value"})";
        ByteBuffer buffer3(str.begin(), str.end());
        REQUIRE(JsonSerializer::is_valid_json(buffer3));

        str = "[1,2,3]";
        ByteBuffer buffer4(str.begin(), str.end());
        REQUIRE(JsonSerializer::is_valid_json(buffer4));
    }

    SECTION("无效的 JSON") {
        // 不完整的 JSON
        ByteBuffer buffer1{'{', '"', 'k', 'e', 'y', ':', '"', 'v', 'a', 'l', 'u', 'e'};
        REQUIRE_FALSE(JsonSerializer::is_valid_json(buffer1));

        ByteBuffer buffer2{'{', 'i', 'n', 'v', 'a', 'l', 'i', 'd', '}'};
        REQUIRE_FALSE(JsonSerializer::is_valid_json(buffer2));

        // 更明确的无效 JSON
        ByteBuffer buffer3{'{', '{', '}'};
        REQUIRE_FALSE(JsonSerializer::is_valid_json(buffer3));

        ByteBuffer buffer4{'n', 'u', 'l', 'l', 'l'};
        REQUIRE_FALSE(JsonSerializer::is_valid_json(buffer4));
    }

    SECTION("空数据") {
        ByteBuffer buffer;
        REQUIRE_FALSE(JsonSerializer::is_valid_json(buffer));
    }
}

TEST_CASE("JsonSerializer get_json_type", "[json][type]") {
    SECTION("整数类型") {
        ByteBuffer buffer{'4', '2'};
        REQUIRE(JsonSerializer::get_json_type(buffer) == "int64");
    }

    SECTION("浮点数类型") {
        ByteBuffer buffer{'3', '.', '1', '4'};
        REQUIRE(JsonSerializer::get_json_type(buffer) == "double");
    }

    SECTION("字符串类型") {
        ByteBuffer buffer{'"', 'h', 'e', 'l', 'l', 'o', '"'};
        REQUIRE(JsonSerializer::get_json_type(buffer) == "string");
    }

    SECTION("布尔类型") {
        ByteBuffer buffer{'t', 'r', 'u', 'e'};
        REQUIRE(JsonSerializer::get_json_type(buffer) == "bool");
    }

    SECTION("数组类型") {
        std::string str = "[1,2,3]";
        ByteBuffer buffer(str.begin(), str.end());
        REQUIRE(JsonSerializer::get_json_type(buffer) == "array");
    }

    SECTION("对象类型") {
        std::string str = R"({"key":"value"})";
        ByteBuffer buffer(str.begin(), str.end());
        REQUIRE(JsonSerializer::get_json_type(buffer) == "object");
    }

    SECTION("空值类型") {
        ByteBuffer buffer{'n', 'u', 'l', 'l'};
        REQUIRE(JsonSerializer::get_json_type(buffer) == "null");
    }
}

TEST_CASE("JsonSerializer get_json_string", "[json][object]") {
    SECTION("获取字符串值") {
        std::string str = R"({"name":"alice"})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_string(buffer, "name");
        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "alice");
    }

    SECTION("键不存在") {
        std::string str = R"({"name":"alice"})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_string(buffer, "missing");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("不是对象类型") {
        std::string str = "123";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_string(buffer, "key");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("类型不匹配") {
        std::string str = R"({"age":30})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_string(buffer, "age");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }
}

TEST_CASE("JsonSerializer get_json_int64", "[json][object]") {
    SECTION("获取整数值") {
        std::string str = R"({"age":30})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_int64(buffer, "age");
        REQUIRE(std::holds_alternative<int64_t>(result));
        REQUIRE(std::get<int64_t>(result) == 30);
    }

    SECTION("键不存在") {
        std::string str = R"({"age":30})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_int64(buffer, "missing");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("类型不匹配") {
        std::string str = R"({"name":"alice"})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_int64(buffer, "name");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }
}

TEST_CASE("JsonSerializer get_json_double", "[json][object]") {
    SECTION("获取浮点数值") {
        std::string str = R"({"price":19.99})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_double(buffer, "price");
        REQUIRE(std::holds_alternative<double>(result));
        REQUIRE(std::get<double>(result) == Catch::Approx(19.99));
    }

    SECTION("键不存在") {
        std::string str = R"({"price":19.99})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_double(buffer, "missing");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("类型不匹配") {
        std::string str = R"({"name":"alice"})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_double(buffer, "name");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }
}

TEST_CASE("JsonSerializer get_json_bool", "[json][object]") {
    SECTION("获取布尔值 true") {
        std::string str = R"({"active":true})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_bool(buffer, "active");
        REQUIRE(std::holds_alternative<bool>(result));
        REQUIRE(std::get<bool>(result) == true);
    }

    SECTION("获取布尔值 false") {
        std::string str = R"({"active":false})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_bool(buffer, "active");
        REQUIRE(std::holds_alternative<bool>(result));
        REQUIRE(std::get<bool>(result) == false);
    }

    SECTION("键不存在") {
        std::string str = R"({"active":true})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_bool(buffer, "missing");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("类型不匹配") {
        std::string str = R"({"name":"alice"})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_bool(buffer, "name");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }
}

TEST_CASE("JsonSerializer get_json_value_type", "[json][object]") {
    SECTION("字符串类型") {
        std::string str = R"({"name":"alice"})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_value_type(buffer, "name");
        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "string");
    }

    SECTION("整数类型") {
        std::string str = R"({"age":30})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_value_type(buffer, "age");
        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "int64");
    }

    SECTION("浮点数类型") {
        std::string str = R"({"price":19.99})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_value_type(buffer, "price");
        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "double");
    }

    SECTION("布尔类型") {
        std::string str = R"({"active":true})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_value_type(buffer, "active");
        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "bool");
    }

    SECTION("空值类型") {
        std::string str = R"({"value":null})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_value_type(buffer, "value");
        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "null");
    }

    SECTION("键不存在") {
        std::string str = R"({"name":"alice"})";
        ByteBuffer buffer(str.begin(), str.end());
        auto result = JsonSerializer::get_json_value_type(buffer, "missing");
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }
}

TEST_CASE("JsonSerializer serialize/deserialize 接口", "[json][interface]") {
    SECTION("serialize 接口") {
        std::string str = R"({"test":"value"})";
        ByteBuffer buffer(str.begin(), str.end());
        
        JsonSerializer serializer;
        auto result = serializer.serialize(buffer);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        REQUIRE(JsonSerializer::is_valid_json(std::get<ByteBuffer>(result)));
    }

    SECTION("deserialize 接口") {
        std::string str = R"({"test":"value"})";
        ByteBuffer buffer(str.begin(), str.end());
        
        JsonSerializer serializer;
        auto result = serializer.deserialize(buffer);
        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        REQUIRE(JsonSerializer::is_valid_json(std::get<ByteBuffer>(result)));
    }

    SECTION("serialize 无效 JSON") {
        // 不完整的 JSON
        ByteBuffer buffer{'{', '"', 'k', 'e', 'y'};

        JsonSerializer serializer;
        auto result = serializer.serialize(buffer);
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }

    SECTION("deserialize 无效 JSON") {
        // 不完整的 JSON
        ByteBuffer buffer{'{', '"', 'k', 'e', 'y'};

        JsonSerializer serializer;
        auto result = serializer.deserialize(buffer);
        REQUIRE(std::holds_alternative<Rendu::Error>(result));
    }
}
