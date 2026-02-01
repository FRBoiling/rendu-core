#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <thread>
#include <chrono>
#include <future>
#include <atomic>

#include "core/cluster/node_discovery.h"

using namespace Rendu;

TEST_CASE("NodeDiscovery 基本构造", "[cluster][node_discovery]") {
    SECTION("默认配置构造") {
        NodeDiscovery::Config config;
        config.node_id = "node-1";
        config.listen_address = "127.0.0.1";
        config.listen_port = 19001;
        config.broadcast_port = 19001;

        NodeDiscovery discovery(config);

        REQUIRE(discovery.get_local_node().node_id == "node-1");
        REQUIRE(discovery.get_local_node().address == "127.0.0.1");
        REQUIRE(discovery.get_local_node().port == 19001);
        REQUIRE(discovery.get_node_count() == 0);
    }

    SECTION("自动生成节点 ID") {
        NodeDiscovery::Config config;
        config.listen_address = "127.0.0.1";
        config.listen_port = 19002;

        NodeDiscovery discovery(config);

        REQUIRE(!discovery.get_local_node().node_id.empty());
        REQUIRE(discovery.get_local_node().node_id.size() == 36);
    }
}

TEST_CASE("NodeDiscovery 启动停止", "[cluster][node_discovery]") {
    SECTION("正常启动停止") {
        NodeDiscovery::Config config;
        config.node_id = "test-node-1";
        config.listen_address = "127.0.0.1";
        config.listen_port = 0;
        config.broadcast_port = 19003;
        config.announce_interval = std::chrono::seconds(1);

        NodeDiscovery discovery(config);

        REQUIRE_NOTHROW(discovery.start());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        REQUIRE_NOTHROW(discovery.stop());
    }

    SECTION("重复启动") {
        NodeDiscovery::Config config;
        config.node_id = "test-node-2";
        config.listen_address = "127.0.0.1";
        config.listen_port = 0;
        config.broadcast_port = 19004;
        config.announce_interval = std::chrono::seconds(1);

        NodeDiscovery discovery(config);

        discovery.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE_NOTHROW(discovery.start());
        discovery.stop();
    }

    SECTION("重复停止") {
        NodeDiscovery::Config config;
        config.node_id = "test-node-3";
        config.listen_address = "127.0.0.1";
        config.listen_port = 0;
        config.broadcast_port = 19005;
        config.announce_interval = std::chrono::seconds(1);

        NodeDiscovery discovery(config);

        discovery.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        discovery.stop();
        REQUIRE_NOTHROW(discovery.stop());
    }
}

TEST_CASE("NodeDiscovery 手动添加节点", "[cluster][node_discovery]") {
    NodeDiscovery::Config config;
    config.node_id = "test-node";
    config.listen_address = "127.0.0.1";
    config.listen_port = 0;

    NodeDiscovery discovery(config);

    SECTION("添加单个节点") {
        NodeInfo node;
        node.node_id = "remote-node-1";
        node.address = "192.168.1.10";
        node.port = 9001;

        discovery.add_node(node);

        REQUIRE(discovery.get_node_count() == 1);

        auto retrieved = discovery.get_node("remote-node-1");
        REQUIRE(retrieved.has_value());
        REQUIRE(retrieved->node_id == "remote-node-1");
        REQUIRE(retrieved->address == "192.168.1.10");
        REQUIRE(retrieved->port == 9001);
    }

    SECTION("添加多个节点") {
        for (int i = 0; i < 5; ++i) {
            NodeInfo node;
            node.node_id = "remote-node-" + std::to_string(i);
            node.address = "192.168.1.10" + std::to_string(i);
            node.port = 9000 + i;
            discovery.add_node(node);
        }

        REQUIRE(discovery.get_node_count() == 5);

        auto active_nodes = discovery.get_active_nodes();
        REQUIRE(active_nodes.size() == 5);
    }

    SECTION("重复添加同一节点") {
        NodeInfo node;
        node.node_id = "duplicate-node";
        node.address = "192.168.1.100";
        node.port = 9001;

        discovery.add_node(node);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        discovery.add_node(node);

        REQUIRE(discovery.get_node_count() == 1);
    }
}

TEST_CASE("NodeDiscovery 节点查询", "[cluster][node_discovery]") {
    NodeDiscovery::Config config;
    config.node_id = "test-node";
    config.listen_address = "127.0.0.1";
    config.listen_port = 0;

    NodeDiscovery discovery(config);

    SECTION("查询存在的节点") {
        NodeInfo node;
        node.node_id = "query-node";
        node.address = "10.0.0.1";
        node.port = 8080;
        discovery.add_node(node);

        auto result = discovery.get_node("query-node");
        REQUIRE(result.has_value());
        REQUIRE(result->node_id == "query-node");
    }

    SECTION("查询不存在的节点") {
        auto result = discovery.get_node("non-existent-node");
        REQUIRE(!result.has_value());
    }

    SECTION("获取活跃节点列表") {
        discovery.add_node(NodeInfo{"node-1", "10.0.0.1", 8001});
        discovery.add_node(NodeInfo{"node-2", "10.0.0.2", 8002});
        discovery.add_node(NodeInfo{"node-3", "10.0.0.3", 8003});

        auto nodes = discovery.get_active_nodes();
        REQUIRE(nodes.size() == 3);
    }
}

TEST_CASE("NodeDiscovery 移除节点", "[cluster][node_discovery]") {
    NodeDiscovery::Config config;
    config.node_id = "test-node";
    config.listen_address = "127.0.0.1";
    config.listen_port = 0;

    NodeDiscovery discovery(config);

    SECTION("移除存在的节点") {
        NodeInfo node;
        node.node_id = "remove-node";
        node.address = "10.0.0.1";
        node.port = 9000;
        discovery.add_node(node);

        REQUIRE(discovery.get_node_count() == 1);

        discovery.remove_node("remove-node");

        REQUIRE(discovery.get_node_count() == 0);
        REQUIRE(!discovery.get_node("remove-node").has_value());
    }

    SECTION("移除不存在的节点") {
        REQUIRE_NOTHROW(discovery.remove_node("non-existent-node"));
        REQUIRE(discovery.get_node_count() == 0);
    }
}

TEST_CASE("NodeDiscovery 节点超时", "[cluster][node_discovery]") {
    NodeDiscovery::Config config;
    config.node_id = "test-node";
    config.listen_address = "127.0.0.1";
    config.listen_port = 0;
    config.node_timeout = std::chrono::seconds(2);
    config.cleanup_interval = std::chrono::seconds(1);

    NodeDiscovery discovery(config);

    SECTION("节点过期自动移除") {
        NodeInfo node;
        node.node_id = "timeout-node";
        node.address = "10.0.0.1";
        node.port = 9000;
        discovery.add_node(node);

        REQUIRE(discovery.get_node_count() == 1);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        REQUIRE(discovery.get_node_count() == 0);
    }

    SECTION("手动清理过期节点") {
        NodeInfo node;
        node.node_id = "manual-cleanup-node";
        node.address = "10.0.0.1";
        node.port = 9000;
        discovery.add_node(node);

        REQUIRE(discovery.get_node_count() == 1);

        std::this_thread::sleep_for(std::chrono::seconds(3));
        discovery.cleanup_expired_nodes();

        REQUIRE(discovery.get_node_count() == 0);
    }
}

TEST_CASE("NodeDiscovery 节点变更回调", "[cluster][node_discovery]") {
    NodeDiscovery::Config config;
    config.node_id = "test-node";
    config.listen_address = "127.0.0.1";
    config.listen_port = 0;

    NodeDiscovery discovery(config);

    SECTION("节点加入事件") {
        std::atomic<int> event_count{0};
        std::string captured_node_id;

        discovery.on_node_change([&](const NodeEvent& event) {
            if (event.type == NodeEvent::Type::Joined) {
                event_count++;
                captured_node_id = event.node_info.node_id;
            }
        });

        NodeInfo node;
        node.node_id = "callback-join-node";
        node.address = "10.0.0.1";
        node.port = 9000;
        discovery.add_node(node);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        REQUIRE(event_count == 1);
        REQUIRE(captured_node_id == "callback-join-node");
    }

    SECTION("节点离开事件") {
        std::atomic<int> event_count{0};

        discovery.on_node_change([&](const NodeEvent& event) {
            if (event.type == NodeEvent::Type::Left) {
                event_count++;
            }
        });

        NodeInfo node;
        node.node_id = "callback-leave-node";
        node.address = "10.0.0.1";
        node.port = 9000;
        discovery.add_node(node);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        discovery.remove_node("callback-leave-node");

        REQUIRE(event_count == 1);
    }

    SECTION("注销回调") {
        std::atomic<int> event_count{0};

        discovery.on_node_change([&](const NodeEvent& event) {
            event_count++;
        });

        discovery.remove_node_change_callback();

        NodeInfo node;
        node.node_id = "no-callback-node";
        node.address = "10.0.0.1";
        node.port = 9000;
        discovery.add_node(node);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        REQUIRE(event_count == 0);
    }
}

TEST_CASE("NodeDiscovery 节点更新", "[cluster][node_discovery]") {
    NodeDiscovery::Config config;
    config.node_id = "test-node";
    config.listen_address = "127.0.0.1";
    config.listen_port = 0;

    NodeDiscovery discovery(config);

    SECTION("更新节点地址") {
        NodeInfo node;
        node.node_id = "update-node";
        node.address = "10.0.0.1";
        node.port = 9000;
        discovery.add_node(node);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        NodeInfo updated_node;
        updated_node.node_id = "update-node";
        updated_node.address = "10.0.0.2";
        updated_node.port = 9001;
        discovery.add_node(updated_node);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        auto retrieved = discovery.get_node("update-node");
        REQUIRE(retrieved.has_value());
        REQUIRE(retrieved->address == "10.0.0.2");
        REQUIRE(retrieved->port == 9001);
        REQUIRE(discovery.get_node_count() == 1);
    }
}

TEST_CASE("NodeDiscovery 并发操作", "[cluster][node_discovery]") {
    NodeDiscovery::Config config;
    config.node_id = "test-node";
    config.listen_address = "127.0.0.1";
    config.listen_port = 0;

    NodeDiscovery discovery(config);

    SECTION("并发添加节点") {
        const int thread_count = 10;
        const int nodes_per_thread = 100;
        std::vector<std::thread> threads;

        std::atomic<int> total_added{0};

        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&discovery, t, nodes_per_thread, &total_added]() {
                for (int i = 0; i < nodes_per_thread; ++i) {
                    NodeInfo node;
                    node.node_id = "concurrent-node-" + std::to_string(t * nodes_per_thread + i);
                    node.address = "10.0." + std::to_string(t) + "." + std::to_string(i);
                    node.port = 8000 + i;
                    discovery.add_node(node);
                    total_added++;
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        REQUIRE(discovery.get_node_count() == thread_count * nodes_per_thread);
        REQUIRE(total_added == thread_count * nodes_per_thread);
    }

    SECTION("并发查询节点") {
        discovery.add_node(NodeInfo{"node-1", "10.0.0.1", 8001});
        discovery.add_node(NodeInfo{"node-2", "10.0.0.2", 8002});
        discovery.add_node(NodeInfo{"node-3", "10.0.0.3", 8003});

        const int query_count = 100;
        std::atomic<int> success_count{0};
        std::vector<std::thread> threads;

        for (int t = 0; t < 5; ++t) {
            threads.emplace_back([&discovery, &success_count, query_count]() {
                for (int i = 0; i < query_count; ++i) {
                    if (discovery.get_node("node-" + std::to_string(i % 3 + 1)).has_value()) {
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

TEST_CASE("NodeDiscovery 广播功能", "[cluster][node_discovery]") {
    SECTION("手动广播") {
        NodeDiscovery::Config config;
        config.node_id = "broadcast-node";
        config.listen_address = "127.0.0.1";
        config.listen_port = 0;
        config.broadcast_port = 19011;
        config.announce_interval = std::chrono::seconds(1);

        NodeDiscovery discovery(config);
        discovery.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        REQUIRE_NOTHROW(discovery.broadcast());

        discovery.stop();
    }
}

TEST_CASE("NodeInfo 过期检查", "[cluster][node_discovery]") {
    SECTION("节点未过期") {
        NodeInfo node;
        node.node_id = "node-1";
        node.address = "10.0.0.1";
        node.port = 9000;
        node.last_seen = std::chrono::system_clock::now();

        REQUIRE(!node.is_expired(std::chrono::seconds(5)));
    }

    SECTION("节点已过期") {
        NodeInfo node;
        node.node_id = "node-1";
        node.address = "10.0.0.1";
        node.port = 9000;
        node.last_seen = std::chrono::system_clock::now() - std::chrono::seconds(10);

        REQUIRE(node.is_expired(std::chrono::seconds(5)));
    }

    SECTION("节点刚好未过期") {
        NodeInfo node;
        node.node_id = "node-1";
        node.address = "10.0.0.1";
        node.port = 9000;
        node.last_seen = std::chrono::system_clock::now() - std::chrono::seconds(5);

        REQUIRE(!node.is_expired(std::chrono::seconds(5)));
    }
}
