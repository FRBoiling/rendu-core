#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <thread>
#include <chrono>
#include <future>
#include <atomic>
#include <unordered_set>

#include "core/cluster/failover_manager.h"

using namespace Rendu;

TEST_CASE("FailoverManager 基本构造", "[cluster][failover_manager]") {
    SECTION("默认配置构造") {
        FailoverManager::Config config;
        FailoverManager fm(config);

        REQUIRE(fm.get_node_count() == 0);
        REQUIRE(fm.get_healthy_node_count() == 0);
    }

    SECTION("自定义配置构造") {
        FailoverManager::Config config;
        config.health_check_interval = std::chrono::seconds(2);
        config.failure_timeout = std::chrono::seconds(10);
        config.max_consecutive_failures = 2;

        FailoverManager fm(config);

        REQUIRE(fm.get_node_count() == 0);
    }
}

TEST_CASE("FailoverManager 添加和移除节点", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(3);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("添加节点") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        fm.add_node(node);

        REQUIRE(fm.get_node_count() == 1);
        REQUIRE(fm.get_healthy_node_count() == 1);

        auto health = fm.get_node_health("node-1");
        REQUIRE(health.has_value());
        REQUIRE(health->node_id == "node-1");
        REQUIRE(health->state == NodeState::Healthy);
    }

    SECTION("添加多个节点") {
        for (int i = 0; i < 5; ++i) {
            LoadBalancedNode node;
            node.node_id = "node-" + std::to_string(i);
            node.address = "192.168.1.1" + std::to_string(i);
            node.port = 9000 + i;

            fm.add_node(node);
        }

        REQUIRE(fm.get_node_count() == 5);
        REQUIRE(fm.get_healthy_node_count() == 5);
    }

    SECTION("移除节点") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        fm.add_node(node);
        REQUIRE(fm.get_node_count() == 1);

        fm.remove_node("node-1");
        REQUIRE(fm.get_node_count() == 0);
    }

    SECTION("移除不存在的节点") {
        REQUIRE_NOTHROW(fm.remove_node("non-existent-node"));
        REQUIRE(fm.get_node_count() == 0);
    }
}

TEST_CASE("FailoverManager 节点故障", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(2);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("手动标记节点失败") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        fm.add_node(node);

        fm.mark_node_failed("node-1", "Manual failure");

        auto health = fm.get_node_health("node-1");
        REQUIRE(health.has_value());
        REQUIRE(health->state == NodeState::Failed);
        REQUIRE(health->last_error == "Manual failure");

        REQUIRE(fm.get_healthy_node_count() == 0);
    }

    SECTION("获取健康节点") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            fm.add_node(nodes[i]);
        }

        fm.mark_node_failed("node-1", "Failed");

        auto healthy_nodes = fm.get_healthy_nodes();
        REQUIRE(healthy_nodes.size() == 2);

        std::unordered_set<std::string> node_ids;
        for (const auto& node : healthy_nodes) {
            node_ids.insert(node.node_id);
        }

        REQUIRE(node_ids.find("node-0") != node_ids.end());
        REQUIRE(node_ids.find("node-1") == node_ids.end());
        REQUIRE(node_ids.find("node-2") != node_ids.end());
    }

    SECTION("获取失败节点") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            fm.add_node(nodes[i]);
        }

        fm.mark_node_failed("node-1", "Failed");
        fm.mark_node_failed("node-2", "Failed");

        auto failed_nodes = fm.get_failed_nodes();
        REQUIRE(failed_nodes.size() == 2);

        std::unordered_set<std::string> node_ids;
        for (const auto& health : failed_nodes) {
            node_ids.insert(health.node_id);
        }

        REQUIRE(node_ids.find("node-1") != node_ids.end());
        REQUIRE(node_ids.find("node-2") != node_ids.end());
    }
}

TEST_CASE("FailoverManager 节点恢复", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(2);
    config.recovery_timeout = std::chrono::seconds(2);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("手动恢复节点") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        fm.add_node(node);
        fm.mark_node_failed("node-1", "Failed");

        auto health = fm.get_node_health("node-1");
        REQUIRE(health->state == NodeState::Failed);

        fm.mark_node_recovered("node-1");

        health = fm.get_node_health("node-1");
        REQUIRE(health->state == NodeState::Healthy);
        REQUIRE(health->consecutive_failures == 0);
    }
}

TEST_CASE("FailoverManager 故障事件回调", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(2);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("节点失败事件") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        std::atomic<bool> event_triggered{false};
        std::string captured_node_id;
        NodeState captured_new_state;

        fm.on_node_failure([&](const NodeFailureEvent& event) {
            event_triggered = true;
            captured_node_id = event.node_id;
            captured_new_state = event.new_state;
        });

        fm.add_node(node);
        fm.mark_node_failed("node-1", "Test failure");

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        REQUIRE(event_triggered == true);
        REQUIRE(captured_node_id == "node-1");
        REQUIRE(captured_new_state == NodeState::Failed);
    }

    SECTION("节点恢复事件") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        std::atomic<bool> recovery_event{false};
        NodeState captured_previous_state;

        fm.on_node_failure([&](const NodeFailureEvent& event) {
            if (event.new_state == NodeState::Healthy) {
                recovery_event = true;
                captured_previous_state = event.previous_state;
            }
        });

        fm.add_node(node);
        fm.mark_node_failed("node-1", "Failed");
        fm.mark_node_recovered("node-1");

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        REQUIRE(recovery_event == true);
        REQUIRE(captured_previous_state == NodeState::Failed);
    }
}

TEST_CASE("FailoverManager 自定义健康检查", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(2);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("自定义健康检查器") {
        LoadBalancedNode node1, node2;
        node1.node_id = "node-1";
        node1.address = "192.168.1.10";
        node1.port = 9001;
        node2.node_id = "node-2";
        node2.address = "192.168.1.20";
        node2.port = 9002;

        // 设置自定义健康检查器
        fm.set_health_checker([](const LoadBalancedNode& node) {
            // node-1 总是失败，node-2 总是成功
            return node.node_id == "node-2";
        });

        fm.add_node(node1);
        fm.add_node(node2);

        // 启动健康检查
        fm.start();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        fm.stop();

        // node-1 应该失败
        auto health1 = fm.get_node_health("node-1");
        REQUIRE(health1.has_value());
        REQUIRE(health1->state != NodeState::Healthy);

        // node-2 应该健康
        auto health2 = fm.get_node_health("node-2");
        REQUIRE(health2.has_value());
        REQUIRE(health2->state == NodeState::Healthy);
    }
}

TEST_CASE("FailoverManager 启动停止", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::milliseconds(100);
    config.failure_timeout = std::chrono::seconds(1);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("正常启动停止") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        fm.add_node(node);

        REQUIRE_NOTHROW(fm.start());

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        REQUIRE_NOTHROW(fm.stop());
    }

    SECTION("重复启动") {
        fm.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE_NOTHROW(fm.start());
        fm.stop();
    }

    SECTION("重复停止") {
        fm.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        fm.stop();
        REQUIRE_NOTHROW(fm.stop());
    }
}

TEST_CASE("FailoverManager 连续失败计数", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(2);
    config.max_consecutive_failures = 3;

    FailoverManager fm(config);

    SECTION("连续失败导致状态变更") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        fm.add_node(node);

        // 设置自定义健康检查器，模拟失败
        fm.set_health_checker([](const LoadBalancedNode& node) {
            return false;
        });

        fm.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 等待一次检查
        fm.stop();

        auto health = fm.get_node_health("node-1");
        REQUIRE(health.has_value());
        REQUIRE(health->consecutive_failures >= 1);
    }
}

TEST_CASE("FailoverManager 并发操作", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(2);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("并发添加节点") {
        const int thread_count = 10;
        const int nodes_per_thread = 50;
        std::vector<std::thread> threads;
        std::atomic<int> total_added{0};

        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&fm, t, nodes_per_thread, &total_added]() {
                for (int i = 0; i < nodes_per_thread; ++i) {
                    LoadBalancedNode node;
                    node.node_id = "node-" + std::to_string(t * nodes_per_thread + i);
                    node.address = "10.0." + std::to_string(t) + "." + std::to_string(i);
                    node.port = 8000 + i;
                    fm.add_node(node);
                    total_added++;
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(fm.get_node_count() == thread_count * nodes_per_thread);
    }

    SECTION("并发查询节点") {
        LoadBalancedNode nodes[5];
        for (int i = 0; i < 5; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            fm.add_node(nodes[i]);
        }

        const int query_count = 100;
        std::atomic<int> success_count{0};
        std::vector<std::thread> threads;

        for (int t = 0; t < 5; ++t) {
            threads.emplace_back([&fm, &success_count, query_count]() {
                for (int i = 0; i < query_count; ++i) {
                    if (fm.get_node_health("node-" + std::to_string(i % 5)).has_value()) {
                        success_count++;
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(success_count == 5 * query_count);
    }
}

TEST_CASE("FailoverManager 手动健康检查", "[cluster][failover_manager]") {
    FailoverManager::Config config;
    config.health_check_interval = std::chrono::seconds(1);
    config.failure_timeout = std::chrono::seconds(2);
    config.max_consecutive_failures = 2;

    FailoverManager fm(config);

    SECTION("手动触发健康检查") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        fm.add_node(node);

        // 设置自定义健康检查器
        fm.set_health_checker([](const LoadBalancedNode& node) {
            return false;
        });

        // 手动触发健康检查
        REQUIRE_NOTHROW(fm.perform_health_check());

        auto health = fm.get_node_health("node-1");
        REQUIRE(health.has_value());
        REQUIRE(health->consecutive_failures > 0);
    }
}
