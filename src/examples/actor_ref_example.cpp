//**********************************
//  ActorRef 序列化示例
//  Created by boil on 2026/02/01.
//**********************************

#include <iostream>
#include <vector>
#include "core/actor/actor_ref.h"

using namespace Rendu;

int main() {
    std::cout << "========== ActorRef 序列化示例 ==========" << std::endl;
    std::cout << std::endl;

    // ============================================================================
    // 1. 基本序列化和反序列化
    // ============================================================================

    std::cout << "1. 基本序列化和反序列化" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    ActorRef actor1("/system/network", 12345);
    std::string serialized = actor1.serialize();

    std::cout << "  原始 ActorRef: " << actor1.to_string() << std::endl;
    std::cout << "  序列化结果: " << serialized << std::endl;

    ActorRef deserialized = ActorRef::deserialize(serialized);
    std::cout << "  反序列化结果: " << deserialized.to_string() << std::endl;
    std::cout << "  相等性检查: " << (actor1 == deserialized ? "通过" : "失败") << std::endl;

    std::cout << std::endl;

    // ============================================================================
    // 2. 特殊字符处理
    // ============================================================================

    std::cout << "2. 特殊字符处理" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    ActorRef actor2("/user:name/123", 67890);
    serialized = actor2.serialize();

    std::cout << "  包含特殊字符的路径: " << actor2.path() << std::endl;
    std::cout << "  序列化结果: " << serialized << std::endl;

    deserialized = ActorRef::deserialize(serialized);
    std::cout << "  反序列化结果: " << deserialized.to_string() << std::endl;
    std::cout << "  路径正确: " << (deserialized.path() == "/user:name/123" ? "是" : "否") << std::endl;

    std::cout << std::endl;

    // ============================================================================
    // 3. 序列化字符串验证
    // ============================================================================

    std::cout << "3. 序列化字符串验证" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    std::vector<std::string> test_strings = {
        "/system/network:12345",  // 有效
        "a:1",                   // 有效
        "/path/with/slashes:999", // 有效
        "",                       // 无效（空）
        "/system/network",         // 无效（无冒号）
        "/system/network:",        // 无效（ID 为空）
        "/system/network:abc",    // 无效（ID 非数字）
        "/system/network:12a34"   // 无效（ID 部分包含非数字）
    };

    for (const auto& str : test_strings) {
        bool valid = ActorRef::is_valid_serialized(str);
        std::cout << "  \"" << (str.empty() ? "(空)" : str) << "\" -> "
                  << (valid ? "有效" : "无效") << std::endl;
    }

    std::cout << std::endl;

    // ============================================================================
    // 4. 错误处理
    // ============================================================================

    std::cout << "4. 错误处理" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    std::vector<std::string> invalid_strings = {
        "",
        "/system/network",
        "/system/network:abc",
        "/system/network:123abc"
    };

    for (const auto& str : invalid_strings) {
        ActorRef ref = ActorRef::deserialize(str);
        std::cout << "  反序列化 \""
                  << (str.empty() ? "(空)" : str) << "\": "
                  << "路径=" << (ref.path().empty() ? "(空)" : ref.path()) << ", "
                  << "ID=" << ref.actor_id() << ", "
                  << "有效=" << (ref.is_valid() ? "是" : "否") << std::endl;
    }

    std::cout << std::endl;

    // ============================================================================
    // 5. 模拟消息传递场景
    // ============================================================================

    std::cout << "5. 模拟消息传递" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    // 创建多个 Actor 引用
    ActorRef sender("/user/alice", 100);
    ActorRef receiver("/service/echo", 200);
    ActorRef router("/system/router", 300);

    // 序列化所有引用（模拟通过网络发送）
    std::vector<std::string> serialized_refs;
    serialized_refs.push_back(sender.serialize());
    serialized_refs.push_back(receiver.serialize());
    serialized_refs.push_back(router.serialize());

    std::cout << "  发送端序列化:" << std::endl;
    for (size_t i = 0; i < serialized_refs.size(); ++i) {
        std::cout << "    Ref " << i << ": " << serialized_refs[i] << std::endl;
    }

    // 接收端反序列化
    std::vector<ActorRef> received_refs;
    for (const auto& s : serialized_refs) {
        received_refs.push_back(ActorRef::deserialize(s));
    }

    std::cout << "  接收端反序列化:" << std::endl;
    std::cout << "    Sender: " << received_refs[0].to_string() << std::endl;
    std::cout << "    Receiver: " << received_refs[1].to_string() << std::endl;
    std::cout << "    Router: " << received_refs[2].to_string() << std::endl;

    // 验证正确性
    bool all_correct = (received_refs[0] == sender) &&
                     (received_refs[1] == receiver) &&
                     (received_refs[2] == router);
    std::cout << "  所有引用正确恢复: " << (all_correct ? "是" : "否") << std::endl;

    std::cout << std::endl;

    // ============================================================================
    // 6. 无效引用处理
    // ============================================================================

    std::cout << "6. 无效引用处理" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    ActorRef invalid_ref1;  // 默认构造
    ActorRef invalid_ref2("/test", 0);  // ID 为 0
    ActorRef invalid_ref3("", 123);  // 路径为空

    std::cout << "  默认构造:" << std::endl;
    std::cout << "    路径: " << (invalid_ref1.path().empty() ? "(空)" : invalid_ref1.path()) << ", "
              << "ID: " << invalid_ref1.actor_id() << ", "
              << "有效: " << (invalid_ref1.is_valid() ? "是" : "否") << std::endl;

    std::cout << "  ID 为 0:" << std::endl;
    std::cout << "    路径: " << invalid_ref2.path() << ", "
              << "ID: " << invalid_ref2.actor_id() << ", "
              << "有效: " << (invalid_ref2.is_valid() ? "是" : "否") << std::endl;

    std::cout << "  路径为空:" << std::endl;
    std::cout << "    路径: " << (invalid_ref3.path().empty() ? "(空)" : invalid_ref3.path()) << ", "
              << "ID: " << invalid_ref3.actor_id() << ", "
              << "有效: " << (invalid_ref3.is_valid() ? "是" : "否") << std::endl;

    std::cout << std::endl;

    // ============================================================================
    // 7. 批量处理
    // ============================================================================

    std::cout << "7. 批量序列化和反序列化" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    std::vector<ActorRef> worker_actors;
    for (int i = 1; i <= 10; ++i) {
        std::string path = "/system/worker" + std::to_string(i);
        worker_actors.emplace_back(path, i * 1000);
    }

    std::vector<std::string> batch_serialized;
    for (const auto& ref : worker_actors) {
        batch_serialized.push_back(ref.serialize());
    }

    std::vector<ActorRef> batch_deserialized;
    for (const auto& s : batch_serialized) {
        batch_deserialized.push_back(ActorRef::deserialize(s));
    }

    bool all_match = true;
    for (size_t i = 0; i < worker_actors.size(); ++i) {
        if (batch_deserialized[i] != worker_actors[i]) {
            all_match = false;
            std::cout << "  Worker " << i << " 不匹配" << std::endl;
            break;
        }
    }

    std::cout << "  批量处理结果: " << (all_match ? "全部正确" : "存在错误") << std::endl;

    std::cout << std::endl;

    // ============================================================================
    // 8. to_string vs serialize 对比
    // ============================================================================

    std::cout << "8. to_string vs serialize 对比" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    ActorRef ref("/test/example", 99999);
    std::cout << "  to_string(): " << ref.to_string() << std::endl;
    std::cout << "  serialize(): " << ref.serialize() << std::endl;
    std::cout << "  区别: to_string 使用 '#', serialize 使用 ':'" << std::endl;

    std::cout << std::endl;
    std::cout << "========== 示例完成 ==========" << std::endl;

    return 0;
}
