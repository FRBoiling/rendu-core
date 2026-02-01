/**
 * @file actor_ref_test.cpp
 * @brief ActorRef 序列化测试
 */

#include <catch2/catch_test_macros.hpp>
#include "core/actor/actor_ref.h"
#include <string>

using namespace Rendu;

// ============================================================================
// 基础序列化测试
// ============================================================================

TEST_CASE("ActorRef 基础序列化", "[actor][ref][serialize]") {
    SECTION("序列化和反序列化") {
        ActorRef original("/system/network", 12345);

        std::string serialized = original.serialize();
        REQUIRE(serialized == "/system/network:12345");

        ActorRef deserialized = ActorRef::deserialize(serialized);
        REQUIRE(deserialized.path() == original.path());
        REQUIRE(deserialized.actor_id() == original.actor_id());
        REQUIRE(deserialized.is_valid());
    }

    SECTION("空路径序列化") {
        ActorRef ref("", 12345);
        REQUIRE_FALSE(ref.is_valid());

        std::string serialized = ref.serialize();
        REQUIRE(serialized == ":12345");

        ActorRef deserialized = ActorRef::deserialize(serialized);
        REQUIRE(deserialized.path().empty());
        REQUIRE(deserialized.actor_id() == 12345);
        REQUIRE_FALSE(deserialized.is_valid());
    }

    SECTION("零 ID 序列化") {
        ActorRef ref("/test", 0);
        REQUIRE_FALSE(ref.is_valid());

        std::string serialized = ref.serialize();
        REQUIRE(serialized == "/test:0");

        ActorRef deserialized = ActorRef::deserialize(serialized);
        REQUIRE(deserialized.path() == "/test");
        REQUIRE(deserialized.actor_id() == 0);
        REQUIRE_FALSE(deserialized.is_valid());
    }
}

// ============================================================================
// 特殊字符测试
// ============================================================================

TEST_CASE("ActorRef 特殊字符序列化", "[actor][ref][serialize]") {
    SECTION("包含冒号的路径") {
        ActorRef ref("/user:name", 100);

        std::string serialized = ref.serialize();
        // 应该从右侧查找最后一个冒号
        REQUIRE(serialized == "/user:name:100");

        ActorRef deserialized = ActorRef::deserialize(serialized);
        REQUIRE(deserialized.path() == "/user:name");
        REQUIRE(deserialized.actor_id() == 100);
    }

    SECTION("包含数字的路径") {
        ActorRef ref("/user123/id456", 789);

        std::string serialized = ref.serialize();
        ActorRef deserialized = ActorRef::deserialize(serialized);

        REQUIRE(deserialized.path() == "/user123/id456");
        REQUIRE(deserialized.actor_id() == 789);
    }

    SECTION("长路径") {
        std::string long_path = "/system/network/session/user/data/item";
        ActorRef ref(long_path, 9999999999ULL);

        std::string serialized = ref.serialize();
        ActorRef deserialized = ActorRef::deserialize(serialized);

        REQUIRE(deserialized.path() == long_path);
        REQUIRE(deserialized.actor_id() == 9999999999ULL);
    }
}

// ============================================================================
// 错误处理测试
// ============================================================================

TEST_CASE("ActorRef 反序列化错误处理", "[actor][ref][serialize]") {
    SECTION("空字符串") {
        ActorRef ref = ActorRef::deserialize("");
        REQUIRE_FALSE(ref.is_valid());
        REQUIRE(ref.path().empty());
        REQUIRE(ref.actor_id() == 0);
    }

    SECTION("只有冒号") {
        ActorRef ref = ActorRef::deserialize(":");
        REQUIRE_FALSE(ref.is_valid());
    }

    SECTION("没有冒号") {
        ActorRef ref = ActorRef::deserialize("/system/network");
        REQUIRE_FALSE(ref.is_valid());
    }

    SECTION("无效的 ID") {
        ActorRef ref = ActorRef::deserialize("/system/network:abc");
        REQUIRE_FALSE(ref.is_valid());
    }

    SECTION("ID 部分包含非数字字符") {
        ActorRef ref = ActorRef::deserialize("/system/network:123abc");
        REQUIRE_FALSE(ref.is_valid());
    }

    SECTION("超大 ID") {
        ActorRef ref = ActorRef::deserialize("/system/network:999999999999999999999999999999999");
        REQUIRE_FALSE(ref.is_valid());
    }
}

// ============================================================================
// 验证函数测试
// ============================================================================

TEST_CASE("ActorRef 验证函数", "[actor][ref][serialize]") {
    SECTION("有效的序列化字符串") {
        REQUIRE(ActorRef::is_valid_serialized("/system/network:12345"));
        REQUIRE(ActorRef::is_valid_serialized("a:1"));
        REQUIRE(ActorRef::is_valid_serialized("/path/with/slashes:999"));
    }

    SECTION("无效的序列化字符串") {
        REQUIRE_FALSE(ActorRef::is_valid_serialized(""));
        REQUIRE_FALSE(ActorRef::is_valid_serialized(":"));
        REQUIRE_FALSE(ActorRef::is_valid_serialized("/system/network"));
        REQUIRE_FALSE(ActorRef::is_valid_serialized("/system/network:"));
        REQUIRE_FALSE(ActorRef::is_valid_serialized("/system/network:abc"));
        REQUIRE_FALSE(ActorRef::is_valid_serialized("/system/network:12a34"));
    }
}

// ============================================================================
// 相等性和哈希测试
// ============================================================================

TEST_CASE("ActorRef 相等性和哈希", "[actor][ref][serialize]") {
    SECTION("序列化后保持相等性") {
        ActorRef original("/test/actor", 42);
        std::string serialized = original.serialize();
        ActorRef deserialized = ActorRef::deserialize(serialized);

        REQUIRE(original == deserialized);
        REQUIRE_FALSE(original != deserialized);
    }

    SECTION("哈希值一致性") {
        ActorRef original("/test/actor", 42);
        std::string serialized = original.serialize();
        ActorRef deserialized = ActorRef::deserialize(serialized);

        REQUIRE(original.hash() == deserialized.hash());
    }

    SECTION("不同引用不相等") {
        ActorRef ref1("/test/actor1", 42);
        ActorRef ref2("/test/actor2", 42);

        REQUIRE(ref1 != ref2);
    }
}

// ============================================================================
// to_string 对比 serialize
// ============================================================================

TEST_CASE("to_string vs serialize", "[actor][ref][serialize]") {
    ActorRef ref("/system/network", 12345);

    SECTION("to_string 使用 # 分隔") {
        REQUIRE(ref.to_string() == "/system/network#12345");
    }

    SECTION("serialize 使用 : 分隔") {
        REQUIRE(ref.serialize() == "/system/network:12345");
    }

    SECTION("两种格式可以区分") {
        std::string str_repr = ref.to_string();
        std::string serial_repr = ref.serialize();

        REQUIRE(str_repr != serial_repr);
        REQUIRE(str_repr.find('#') != std::string::npos);
        REQUIRE(serial_repr.find(':') != std::string::npos);
    }
}

// ============================================================================
// 综合场景测试
// ============================================================================

TEST_CASE("综合场景测试", "[actor][ref][serialize]") {
    SECTION("模拟消息传递序列化") {
        // 创建 Actor 引用
        ActorRef sender("/user/alice", 100);
        ActorRef receiver("/service/echo", 200);

        // 序列化引用（模拟通过网络发送）
        std::string sender_serialized = sender.serialize();
        std::string receiver_serialized = receiver.serialize();

        // 接收端反序列化
        ActorRef received_sender = ActorRef::deserialize(sender_serialized);
        ActorRef received_receiver = ActorRef::deserialize(receiver_serialized);

        // 验证引用正确恢复
        REQUIRE(received_sender == sender);
        REQUIRE(received_receiver == receiver);
    }

    SECTION("批量序列化和反序列化") {
        std::vector<ActorRef> refs = {
            ActorRef("/system/worker1", 1),
            ActorRef("/system/worker2", 2),
            ActorRef("/system/worker3", 3),
            ActorRef("/system/worker4", 4),
            ActorRef("/system/worker5", 5)
        };

        std::vector<std::string> serialized;
        for (const auto& ref : refs) {
            serialized.push_back(ref.serialize());
        }

        std::vector<ActorRef> deserialized;
        for (const auto& s : serialized) {
            deserialized.push_back(ActorRef::deserialize(s));
        }

        REQUIRE(deserialized.size() == refs.size());
        for (size_t i = 0; i < refs.size(); ++i) {
            REQUIRE(deserialized[i] == refs[i]);
        }
    }
}
