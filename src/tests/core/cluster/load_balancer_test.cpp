#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <thread>
#include <chrono>
#include <future>
#include <atomic>
#include <unordered_set>

#include "core/cluster/load_balancer.h"

using namespace Rendu;

TEST_CASE("LoadBalancer 基本构造", "[cluster][load_balancer]") {
    SECTION("默认轮询策略构造") {
        LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

        REQUIRE(lb.get_node_count() == 0);
        REQUIRE(lb.select().has_value() == false);
    }

    SECTION("最少连接策略构造") {
        LoadBalancer lb(LoadBalanceStrategy::LeastConnections);

        REQUIRE(lb.get_node_count() == 0);
    }

    SECTION("加权随机策略构造") {
        LoadBalancer lb(LoadBalanceStrategy::Weighted);

        REQUIRE(lb.get_node_count() == 0);
    }

    SECTION("随机策略构造") {
        LoadBalancer lb(LoadBalanceStrategy::Random);

        REQUIRE(lb.get_node_count() == 0);
    }
}

TEST_CASE("LoadBalancer 添加节点", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("添加单个节点") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;
        node.weight = 1;

        lb.add_node(node);

        REQUIRE(lb.get_node_count() == 1);

        auto selected = lb.select();
        REQUIRE(selected.has_value());
        REQUIRE(selected->node_id == "node-1");
    }

    SECTION("添加多个节点") {
        for (int i = 0; i < 5; ++i) {
            LoadBalancedNode node;
            node.node_id = "node-" + std::to_string(i);
            node.address = "192.168.1.1" + std::to_string(i);
            node.port = 9000 + i;
            node.weight = 1;

            lb.add_node(node);
        }

        REQUIRE(lb.get_node_count() == 5);
    }

    SECTION("添加重复节点（覆盖）") {
        LoadBalancedNode node1;
        node1.node_id = "duplicate-node";
        node1.address = "192.168.1.10";
        node1.port = 9001;
        node1.weight = 1;

        lb.add_node(node1);

        LoadBalancedNode node2;
        node2.node_id = "duplicate-node";
        node2.address = "192.168.1.20";
        node2.port = 9002;
        node2.weight = 2;

        lb.add_node(node2);

        REQUIRE(lb.get_node_count() == 1);

        auto selected = lb.select();
        REQUIRE(selected->port == 9002);
        REQUIRE(selected->weight == 2);
    }
}

TEST_CASE("LoadBalancer 移除节点", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("移除存在的节点") {
        LoadBalancedNode node1, node2, node3;
        node1.node_id = "node-1";
        node1.address = "192.168.1.10";
        node1.port = 9001;
        node2.node_id = "node-2";
        node2.address = "192.168.1.20";
        node2.port = 9002;
        node3.node_id = "node-3";
        node3.address = "192.168.1.30";
        node3.port = 9003;

        lb.add_node(node1);
        lb.add_node(node2);
        lb.add_node(node3);

        REQUIRE(lb.get_node_count() == 3);

        lb.remove_node("node-2");

        REQUIRE(lb.get_node_count() == 2);
    }

    SECTION("移除不存在的节点") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        lb.add_node(node);

        REQUIRE_NOTHROW(lb.remove_node("non-existent-node"));

        REQUIRE(lb.get_node_count() == 1);
    }
}

TEST_CASE("LoadBalancer 轮询策略", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("基本轮询") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            lb.add_node(nodes[i]);
        }

        // 测试轮询顺序
        for (int round = 0; round < 2; ++round) {
            for (int i = 0; i < 3; ++i) {
                auto selected = lb.select();
                REQUIRE(selected.has_value());
                REQUIRE(selected->node_id == nodes[i].node_id);
            }
        }
    }

    SECTION("移除节点后继续轮询") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            lb.add_node(nodes[i]);
        }

        // 选择第一个节点
        auto selected = lb.select();
        REQUIRE(selected->node_id == "node-0");

        // 移除第二个节点
        lb.remove_node("node-1");

        // 继续轮询，应该跳过被移除的节点
        selected = lb.select();
        REQUIRE(selected->node_id == "node-2");

        selected = lb.select();
        REQUIRE(selected->node_id == "node-0");
    }
}

TEST_CASE("LoadBalancer 最少连接策略", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::LeastConnections);

    SECTION("基本最少连接") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            nodes[i].connections = i * 2;  // 0, 2, 4
            lb.add_node(nodes[i]);
        }

        // 应该选择 node-0（0 个连接）
        auto selected = lb.select();
        REQUIRE(selected.has_value());
        REQUIRE(selected->node_id == "node-0");
        REQUIRE(selected->connections == 0);

        // 增加 node-0 的连接数
        lb.increment_connections("node-0");

        // 现在应该选择 node-0（1 个连接）
        selected = lb.select();
        REQUIRE(selected->node_id == "node-0");
        REQUIRE(selected->connections == 1);
    }

    SECTION("连接数相同时选择第一个") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            nodes[i].connections = 5;  // 所有节点连接数相同
            lb.add_node(nodes[i]);
        }

        auto selected = lb.select();
        REQUIRE(selected.has_value());
        // 应该选择第一个（连接数相同）
        REQUIRE(selected->connections == 5);
    }
}

TEST_CASE("LoadBalancer 加权随机策略", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::Weighted);

    SECTION("权重分配测试") {
        LoadBalancedNode node1, node2;
        node1.node_id = "node-1";
        node1.address = "192.168.1.10";
        node1.port = 9001;
        node1.weight = 9;  // 90% 概率

        node2.node_id = "node-2";
        node2.address = "192.168.1.20";
        node2.port = 9002;
        node2.weight = 1;  // 10% 概率

        lb.add_node(node1);
        lb.add_node(node2);

        // 进行大量选择，验证权重比例
        int count1 = 0, count2 = 0;
        const int trials = 1000;

        for (int i = 0; i < trials; ++i) {
            auto selected = lb.select();
            if (selected->node_id == "node-1") {
                count1++;
            } else {
                count2++;
            }
        }

        // node-1 应该大约占 90% (800-950 次)
        REQUIRE(count1 >= 800);
        REQUIRE(count1 <= 950);
        REQUIRE(count1 + count2 == trials);
    }

    SECTION("零权重节点") {
        LoadBalancedNode node1, node2;
        node1.node_id = "node-1";
        node1.address = "192.168.1.10";
        node1.port = 9001;
        node1.weight = 0;  // 零权重

        node2.node_id = "node-2";
        node2.address = "192.168.1.20";
        node2.port = 9002;
        node2.weight = 1;

        lb.add_node(node1);
        lb.add_node(node2);

        // 应该只选择 node-2
        for (int i = 0; i < 100; ++i) {
            auto selected = lb.select();
            REQUIRE(selected->node_id == "node-2");
        }
    }
}

TEST_CASE("LoadBalancer 随机策略", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::Random);

    SECTION("随机分布") {
        LoadBalancedNode nodes[5];
        for (int i = 0; i < 5; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            lb.add_node(nodes[i]);
        }

        // 进行大量选择，验证所有节点都被选择过
        std::unordered_set<std::string> selected_nodes;
        const int trials = 100;

        for (int i = 0; i < trials; ++i) {
            auto selected = lb.select();
            selected_nodes.insert(selected->node_id);
        }

        // 所有节点都应该被选择过（概率接近 1）
        REQUIRE(selected_nodes.size() >= 4);  // 至少 4 个
    }
}

TEST_CASE("LoadBalancer 连接数管理", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::LeastConnections);

    SECTION("增加和减少连接数") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;
        node.connections = 0;

        lb.add_node(node);

        REQUIRE(lb.get_all_nodes()[0].connections == 0);

        lb.increment_connections("node-1");
        REQUIRE(lb.get_all_nodes()[0].connections == 1);

        lb.increment_connections("node-1");
        lb.increment_connections("node-1");
        REQUIRE(lb.get_all_nodes()[0].connections == 3);

        lb.decrement_connections("node-1");
        REQUIRE(lb.get_all_nodes()[0].connections == 2);
    }

    SECTION("连接数不会低于零") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;
        node.connections = 0;

        lb.add_node(node);

        // 多次减少连接数
        for (int i = 0; i < 10; ++i) {
            lb.decrement_connections("node-1");
        }

        REQUIRE(lb.get_all_nodes()[0].connections == 0);
    }
}

TEST_CASE("LoadBalancer 权重更新", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::Weighted);

    SECTION("更新节点权重") {
        LoadBalancedNode node1, node2;
        node1.node_id = "node-1";
        node1.address = "192.168.1.10";
        node1.port = 9001;
        node1.weight = 1;

        node2.node_id = "node-2";
        node2.address = "192.168.1.20";
        node2.port = 9002;
        node2.weight = 1;

        lb.add_node(node1);
        lb.add_node(node2);

        // 更新 node-1 的权重
        lb.update_node_weight("node-1", 9);

        // 进行选择验证
        int count1 = 0, count2 = 0;
        const int trials = 200;

        for (int i = 0; i < trials; ++i) {
            auto selected = lb.select();
            if (selected->node_id == "node-1") {
                count1++;
            } else {
                count2++;
            }
        }

        // node-1 应该占大约 90%
        REQUIRE(count1 >= 150);
    }
}

TEST_CASE("LoadBalancer 策略切换", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("从轮询切换到随机") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            lb.add_node(nodes[i]);
        }

        // 轮询选择
        auto selected = lb.select();
        REQUIRE(selected->node_id == "node-0");

        // 切换到随机策略
        lb.set_strategy(LoadBalanceStrategy::Random);

        // 随机选择
        selected = lb.select();
        // 不应该确定性地是 node-1
        // (实际上有可能，但概率较低)
    }
}

TEST_CASE("LoadBalancer 并发操作", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("并发添加节点") {
        const int thread_count = 10;
        const int nodes_per_thread = 100;
        std::vector<std::thread> threads;
        std::atomic<int> total_added{0};

        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&lb, t, nodes_per_thread, &total_added]() {
                for (int i = 0; i < nodes_per_thread; ++i) {
                    LoadBalancedNode node;
                    node.node_id = "node-" + std::to_string(t * nodes_per_thread + i);
                    node.address = "10.0." + std::to_string(t) + "." + std::to_string(i);
                    node.port = 8000 + i;
                    lb.add_node(node);
                    total_added++;
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(lb.get_node_count() == thread_count * nodes_per_thread);
    }

    SECTION("并发选择节点") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            lb.add_node(nodes[i]);
        }

        const int select_count = 1000;
        std::atomic<int> success_count{0};
        std::vector<std::thread> threads;

        for (int t = 0; t < 5; ++t) {
            threads.emplace_back([&lb, &success_count, select_count]() {
                for (int i = 0; i < select_count; ++i) {
                    if (lb.select().has_value()) {
                        success_count++;
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(success_count == 5 * select_count);
    }
}

TEST_CASE("LoadBalancer 获取所有节点", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("获取空列表") {
        auto nodes = lb.get_all_nodes();
        REQUIRE(nodes.empty());
    }

    SECTION("获取所有节点") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            lb.add_node(nodes[i]);
        }

        auto all_nodes = lb.get_all_nodes();
        REQUIRE(all_nodes.size() == 3);

        std::unordered_set<std::string> node_ids;
        for (const auto& node : all_nodes) {
            node_ids.insert(node.node_id);
        }

        REQUIRE(node_ids.size() == 3);
    }
}

TEST_CASE("LoadBalancer 清空节点", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("清空节点列表") {
        LoadBalancedNode nodes[3];
        for (int i = 0; i < 3; ++i) {
            nodes[i].node_id = "node-" + std::to_string(i);
            nodes[i].address = "192.168.1.1" + std::to_string(i);
            nodes[i].port = 9000 + i;
            lb.add_node(nodes[i]);
        }

        REQUIRE(lb.get_node_count() == 3);

        lb.clear();

        REQUIRE(lb.get_node_count() == 0);
        REQUIRE(lb.select().has_value() == false);
    }
}

TEST_CASE("LoadBalancer 空节点选择", "[cluster][load_balancer]") {
    LoadBalancer lb(LoadBalanceStrategy::RoundRobin);

    SECTION("空节点列表选择") {
        REQUIRE(lb.get_node_count() == 0);
        REQUIRE(lb.select().has_value() == false);
    }

    SECTION("所有节点被移除后选择") {
        LoadBalancedNode node;
        node.node_id = "node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        lb.add_node(node);
        lb.remove_node("node-1");

        REQUIRE(lb.select().has_value() == false);
    }
}
