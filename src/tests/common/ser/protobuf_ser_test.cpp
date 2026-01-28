//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <common/ser/protobuf_ser.h>
#include "test_messages.pb.h"
#include <random>

using namespace Rendu;
using namespace Rendu::ser;
using Rendu::net::ByteBuffer;

TEST_CASE("ProtobufSerializer - TestMessage 基本序列化", "[ser][protobuf]") {
    TestMessage msg;
    msg.set_id(123);
    msg.set_name("test_message");
    msg.set_timestamp(1234567890);

    SECTION("序列化") {
        auto result = ProtobufSerializer::serialize_message(msg);

        REQUIRE(std::holds_alternative<ByteBuffer>(result));
        auto data = std::get<ByteBuffer>(result);
        REQUIRE(data.size() > 0);
        REQUIRE(data.size() == ProtobufSerializer::message_size(msg));
    }

    SECTION("反序列化") {
        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));
        auto data = std::get<ByteBuffer>(serialize_result);

        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.id() == msg.id());
        REQUIRE(decoded.name() == msg.name());
        REQUIRE(decoded.timestamp() == msg.timestamp());
    }

    SECTION("指针版本反序列化") {
        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));
        auto data = std::get<ByteBuffer>(serialize_result);

        test::TestMessage decoded;
        auto error = ProtobufSerializer::deserialize_message(data, &decoded);
        REQUIRE(error.is_success());

        REQUIRE(decoded.id() == msg.id());
        REQUIRE(decoded.name() == msg.name());
        REQUIRE(decoded.timestamp() == msg.timestamp());
    }
}

TEST_CASE("ProtobufSerializer - TestMessage 重复字段", "[ser][protobuf]") {
    test::TestMessage msg;
    msg.set_id(456);
    msg.set_name("repeated_fields");

    SECTION("序列化和反序列化重复字段") {
        msg.add_values(10);
        msg.add_values(20);
        msg.add_values(30);
        msg.add_values(40);

        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));
        auto data = std::get<ByteBuffer>(serialize_result);

        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.values_size() == 4);
        REQUIRE(decoded.values(0) == 10);
        REQUIRE(decoded.values(1) == 20);
        REQUIRE(decoded.values(2) == 30);
        REQUIRE(decoded.values(3) == 40);
    }

    SECTION("空重复字段") {
        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(
            std::get<ByteBuffer>(serialize_result));
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.values_size() == 0);
    }
}

TEST_CASE("ProtobufSerializer - NestedMessage 嵌套消息", "[ser][protobuf]") {
    test::NestedMessage msg;
    msg.set_title("nested_test");

    test::TestMessage* inner = msg.mutable_inner();
    inner->set_id(789);
    inner->set_name("inner_message");
    inner->set_timestamp(9876543210);
    inner->add_values(100);
    inner->add_values(200);

    SECTION("序列化和反序列化嵌套消息") {
        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));
        auto data = std::get<ByteBuffer>(serialize_result);

        auto deserialize_result = ProtobufSerializer::deserialize_message<test::NestedMessage>(data);
        REQUIRE(std::holds_alternative<test::NestedMessage>(deserialize_result));

        auto decoded = std::get<test::NestedMessage>(deserialize_result);
        REQUIRE(decoded.title() == msg.title());
        REQUIRE(decoded.has_inner());
        REQUIRE(decoded.inner().id() == msg.inner().id());
        REQUIRE(decoded.inner().name() == msg.inner().name());
        REQUIRE(decoded.inner().timestamp() == msg.inner().timestamp());
        REQUIRE(decoded.inner().values_size() == 2);
    }
}

TEST_CASE("ProtobufSerializer - ComplexMessage 复杂消息", "[ser][protobuf]") {
    test::ComplexMessage msg;
    msg.set_id(999);
    msg.set_name("complex_message");
    msg.set_status(test::Status::ACTIVE);

    SECTION("包含重复消息") {
        test::TestMessage* m1 = msg.add_messages();
        m1->set_id(1);
        m1->set_name("msg1");

        test::TestMessage* m2 = msg.add_messages();
        m2->set_id(2);
        m2->set_name("msg2");

        SECTION("序列化和反序列化") {
            auto serialize_result = ProtobufSerializer::serialize_message(msg);
            REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

            auto deserialize_result = ProtobufSerializer::deserialize_message<test::ComplexMessage>(
                std::get<ByteBuffer>(serialize_result));
            REQUIRE(std::holds_alternative<test::ComplexMessage>(deserialize_result));

            auto decoded = std::get<test::ComplexMessage>(deserialize_result);
            REQUIRE(decoded.id() == msg.id());
            REQUIRE(decoded.name() == msg.name());
            REQUIRE(decoded.status() == msg.status());
            REQUIRE(decoded.messages_size() == 2);
            REQUIRE(decoded.messages(0).id() == 1);
            REQUIRE(decoded.messages(1).name() == "msg2");
        }
    }

    SECTION("包含 map 字段") {
        (*msg.mutable_metadata())["key1"] = 100;
        (*msg.mutable_metadata())["key2"] = 200;
        (*msg.mutable_metadata())["key3"] = 300;

        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto deserialize_result = ProtobufSerializer::deserialize_message<test::ComplexMessage>(
            std::get<ByteBuffer>(serialize_result));
        REQUIRE(std::holds_alternative<test::ComplexMessage>(deserialize_result));

        auto decoded = std::get<test::ComplexMessage>(deserialize_result);
        REQUIRE(decoded.metadata_size() == 3);
        REQUIRE(decoded.metadata().at("key1") == 100);
        REQUIRE(decoded.metadata().at("key2") == 200);
        REQUIRE(decoded.metadata().at("key3") == 300);
    }
}

TEST_CASE("ProtobufSerializer - 枚举类型", "[ser][protobuf]") {
    test::ComplexMessage msg;
    msg.set_id(1);
    msg.set_name("enum_test");

    SECTION("所有枚举值") {
        std::vector<test::Status> statuses = {
            test::Status::UNKNOWN,
            test::Status::ACTIVE,
            test::Status::INACTIVE
        };

        for (auto status : statuses) {
            msg.set_status(status);

            auto serialize_result = ProtobufSerializer::serialize_message(msg);
            REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

            auto deserialize_result = ProtobufSerializer::deserialize_message<test::ComplexMessage>(
                std::get<ByteBuffer>(serialize_result));
            REQUIRE(std::holds_alternative<test::ComplexMessage>(deserialize_result));

            auto decoded = std::get<test::ComplexMessage>(deserialize_result);
            REQUIRE(decoded.status() == status);
        }
    }
}

TEST_CASE("ProtobufSerializer - message_size", "[ser][protobuf]") {
    test::TestMessage msg;
    msg.set_id(1);
    msg.set_name("size_test");

    SECTION("不同大小的消息") {
        size_t size1 = ProtobufSerializer::message_size(msg);
        REQUIRE(size1 > 0);

        msg.add_values(1);
        size_t size2 = ProtobufSerializer::message_size(msg);
        REQUIRE(size2 > size1);

        msg.add_values(2);
        msg.add_values(3);
        size_t size3 = ProtobufSerializer::message_size(msg);
        REQUIRE(size3 > size2);

        msg.set_name("a_very_long_name_for_testing_size_calculation");
        size_t size4 = ProtobufSerializer::message_size(msg);
        REQUIRE(size4 > size3);
    }
}

TEST_CASE("ProtobufSerializer - is_valid_message", "[ser][protobuf]") {
    test::TestMessage msg;
    msg.set_id(1);
    msg.set_name("validation_test");

    SECTION("有效消息") {
        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto data = std::get<ByteBuffer>(serialize_result);
        REQUIRE(ProtobufSerializer::is_valid_message<test::TestMessage>(data));
    }

    SECTION("空数据 - Protobuf 允许解析空数据为默认消息") {
        ByteBuffer empty_data;
        // Protobuf 可以解析空数据为默认消息
        REQUIRE(ProtobufSerializer::is_valid_message<test::TestMessage>(empty_data));
    }

    SECTION("无效数据") {
        ByteBuffer invalid_data = {0x00, 0x01, 0x02, 0x03};
        REQUIRE_FALSE(ProtobufSerializer::is_valid_message<test::TestMessage>(invalid_data));
    }
}

TEST_CASE("ProtobufSerializer - 大数据量", "[ser][protobuf]") {
    test::TestMessage msg;
    msg.set_id(1);
    msg.set_name("large_data_test");

    SECTION("大量重复字段") {
        const int COUNT = 1000;
        for (int i = 0; i < COUNT; ++i) {
            msg.add_values(i);
        }

        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto data = std::get<ByteBuffer>(serialize_result);
        REQUIRE(data.size() > 0);

        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.values_size() == COUNT);

        for (int i = 0; i < COUNT; ++i) {
            REQUIRE(decoded.values(i) == i);
        }
    }

    SECTION("长字符串") {
        std::string long_string(10000, 'x');
        msg.set_name(long_string);

        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto data = std::get<ByteBuffer>(serialize_result);
        REQUIRE(data.size() > 0);

        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.name() == long_string);
    }
}

TEST_CASE("ProtobufSerializer - 边界情况", "[ser][protobuf]") {
    test::TestMessage msg;

    SECTION("默认值") {
        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto data = std::get<ByteBuffer>(serialize_result);
        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.id() == 0);  // int32 默认值
        REQUIRE(decoded.name().empty());  // string 默认值
        REQUIRE(decoded.timestamp() == 0);  // int64 默认值
    }

    SECTION("零值") {
        msg.set_id(0);
        msg.set_timestamp(0);

        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto data = std::get<ByteBuffer>(serialize_result);
        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.id() == 0);
        REQUIRE(decoded.timestamp() == 0);
    }

    SECTION("负数") {
        msg.set_id(-123);
        msg.set_timestamp(-456789);

        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto data = std::get<ByteBuffer>(serialize_result);
        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.id() == -123);
        REQUIRE(decoded.timestamp() == -456789);
    }

    SECTION("特殊字符字符串") {
        msg.set_name("特殊字符\n\r\t中文🎉");

        auto serialize_result = ProtobufSerializer::serialize_message(msg);
        REQUIRE(std::holds_alternative<ByteBuffer>(serialize_result));

        auto data = std::get<ByteBuffer>(serialize_result);
        auto deserialize_result = ProtobufSerializer::deserialize_message<test::TestMessage>(data);
        REQUIRE(std::holds_alternative<test::TestMessage>(deserialize_result));

        auto decoded = std::get<test::TestMessage>(deserialize_result);
        REQUIRE(decoded.name() == "特殊字符\n\r\t中文🎉");
    }
}

TEST_CASE("ProtobufSerializer - 错误处理", "[ser][protobuf]") {
    test::TestMessage msg;
    msg.set_id(1);
    msg.set_name("error_test");

    SECTION("无效数据反序列化") {
        ByteBuffer invalid_data = {0xFF, 0xFF, 0xFF, 0xFF};
        auto result = ProtobufSerializer::deserialize_message<test::TestMessage>(invalid_data);
        REQUIRE(std::holds_alternative<Error>(result));
        auto error = std::get<Error>(result);
        REQUIRE(error.code() == ErrorCode::SerializationError);
    }

    SECTION("空数据反序列化 - Protobuf 允许解析空数据为默认消息") {
        ByteBuffer empty_data;
        // Protobuf 可以解析空数据为默认消息
        auto result = ProtobufSerializer::deserialize_message<test::TestMessage>(empty_data);
        REQUIRE(std::holds_alternative<test::TestMessage>(result));
    }
}
