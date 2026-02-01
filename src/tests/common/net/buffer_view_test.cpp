#include <catch2/catch_test_macros.hpp>
#include "common/net/buffer_view.h"
#include "common/net/channel.h"
#include "common/net/codec.h"
#include "common/net/connection_pool.h"
#include <thread>
#include <atomic>

using namespace Rendu;
using namespace Rendu::net;
using namespace Rendu::io;

// 全局 IoContext,避免生命周期问题
static IoContext g_io(2);
static std::shared_ptr<LengthPrefixCodec> g_codec = std::make_shared<LengthPrefixCodec>();

TEST_CASE("BufferView - 基本构造", "[net][buffer_view]") {
    SECTION("默认构造") {
        BufferView view;
        REQUIRE(view.data() == nullptr);
        REQUIRE(view.size() == 0);
        REQUIRE(view.empty());
    }

    SECTION("从指针和大小构造") {
        byte data[] = {0x01, 0x02, 0x03, 0x04};
        BufferView view(data, sizeof(data));

        REQUIRE(view.data() == data);
        REQUIRE(view.size() == 4);
        REQUIRE_FALSE(view.empty());
    }

    SECTION("从 ByteBuffer 构造") {
        ByteBuffer buffer = {0x01, 0x02, 0x03};
        BufferView view(buffer);

        REQUIRE(view.data() == buffer.data());
        REQUIRE(view.size() == 3);
        REQUIRE_FALSE(view.empty());
    }

    SECTION("从指针范围构造") {
        byte data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
        BufferView view(data, data + 3);

        REQUIRE(view.data() == data);
        REQUIRE(view.size() == 3);
        REQUIRE_FALSE(view.empty());
    }

    SECTION("从 string_view 构造") {
        std::string_view sv = "Hello";
        BufferView view(sv);

        REQUIRE(view.size() == 5);
        REQUIRE_FALSE(view.empty());
        REQUIRE(view.to_string_view() == sv);
    }
}

TEST_CASE("BufferView - 转换", "[net][buffer_view]") {
    byte data[] = {0x01, 0x02, 0x03};
    BufferView view(data, sizeof(data));

    SECTION("转换为 ByteBuffer") {
        ByteBuffer buffer = view.to_buffer();

        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == 0x01);
        REQUIRE(buffer[1] == 0x02);
        REQUIRE(buffer[2] == 0x03);
    }

    SECTION("转换为 string_view") {
        std::string_view sv = view.to_string_view();

        REQUIRE(sv.size() == 3);
        REQUIRE(sv[0] == static_cast<char>(0x01));
        REQUIRE(sv[1] == static_cast<char>(0x02));
        REQUIRE(sv[2] == static_cast<char>(0x03));
    }
}

TEST_CASE("BufferView - 子视图", "[net][buffer_view]") {
    byte data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    BufferView view(data, sizeof(data));

    SECTION("正常子视图") {
        BufferView subview = view.subview(1, 3);

        REQUIRE(subview.size() == 3);
        REQUIRE(subview[0] == 0x02);
        REQUIRE(subview[1] == 0x03);
        REQUIRE(subview[2] == 0x04);
    }

    SECTION("偏移量超出范围") {
        BufferView subview = view.subview(10, 3);

        REQUIRE(subview.empty());
        REQUIRE(subview.size() == 0);
    }

    SECTION("长度超出范围") {
        BufferView subview = view.subview(2, 10);

        REQUIRE(subview.size() == 3); // 应该截断到实际可用大小
        REQUIRE(subview[0] == 0x03);
    }
}

TEST_CASE("BufferView - 迭代器", "[net][buffer_view]") {
    byte data[] = {0x01, 0x02, 0x03};
    BufferView view(data, sizeof(data));

    SECTION("使用迭代器遍历") {
        size_t count = 0;
        for (const byte& b : view) {
            REQUIRE(b == data[count]);
            count++;
        }
        REQUIRE(count == 3);
    }
}

TEST_CASE("MutableBufferView - 基本构造", "[net][buffer_view]") {
    SECTION("默认构造") {
        MutableBufferView view;
        REQUIRE(view.data() == nullptr);
        REQUIRE(view.size() == 0);
        REQUIRE(view.empty());
    }

    SECTION("从指针和大小构造") {
        byte data[4] = {0};
        MutableBufferView view(data, sizeof(data));

        REQUIRE(view.data() == data);
        REQUIRE(view.size() == 4);
        REQUIRE_FALSE(view.empty());
    }

    SECTION("从 ByteBuffer 构造") {
        ByteBuffer buffer = {0x01, 0x02, 0x03};
        MutableBufferView view(buffer);

        REQUIRE(view.data() == buffer.data());
        REQUIRE(view.size() == 3);
        REQUIRE_FALSE(view.empty());
    }
}

TEST_CASE("MutableBufferView - 可修改性", "[net][buffer_view]") {
    byte data[3] = {0x01, 0x02, 0x03};
    MutableBufferView view(data, sizeof(data));

    SECTION("修改数据") {
        view[0] = 0xFF;
        view[1] = 0xFE;
        view[2] = 0xFD;

        REQUIRE(data[0] == 0xFF);
        REQUIRE(data[1] == 0xFE);
        REQUIRE(data[2] == 0xFD);
    }

    SECTION("通过迭代器修改") {
        byte value = 0x10;
        for (byte& b : view) {
            b = value++;
        }

        REQUIRE(data[0] == 0x10);
        REQUIRE(data[1] == 0x11);
        REQUIRE(data[2] == 0x12);
    }
}

TEST_CASE("MutableBufferView - 转换", "[net][buffer_view]") {
    byte data[3] = {0x01, 0x02, 0x03};
    MutableBufferView view(data, sizeof(data));

    SECTION("转换为 BufferView") {
        BufferView buffer_view = view.to_view();

        REQUIRE(buffer_view.data() == view.data());
        REQUIRE(buffer_view.size() == view.size());
    }
}

TEST_CASE("Channel - 零拷贝发送", "[net][channel]") {
    log::init_default_io_context(g_io);

    ConnectionPool pool(g_io, "localhost", 0, ConnectionPoolConfig{
        .codec = g_codec
    });

    SECTION("send_zero_copy 基本功能") {
        // 注意: 这里使用 localhost:0 作为测试
        // 由于没有真实的服务器,我们测试代码编译和基本逻辑

        byte data[] = {0x01, 0x02, 0x03};
        BufferView view(data, sizeof(data));

        // 创建 Channel (由于没有服务器,连接会失败)
        auto channel = ChannelFactory::create_client(g_io, "localhost", 0, g_codec);

        // 测试 send_zero_copy 编译和调用
        // 注意: 这不会真正发送数据,只是测试接口
        channel->send_zero_copy(view);
    }

    SECTION("send_batch_zero_copy 基本功能") {
        std::vector<BufferView> views;

        byte data1[] = {0x01, 0x02};
        byte data2[] = {0x03, 0x04};
        byte data3[] = {0x05, 0x06};

        views.emplace_back(data1, sizeof(data1));
        views.emplace_back(data2, sizeof(data2));
        views.emplace_back(data3, sizeof(data3));

        // 创建 Channel
        auto channel = ChannelFactory::create_client(g_io, "localhost", 0, g_codec);

        // 测试 send_batch_zero_copy 编译和调用
        channel->send_batch_zero_copy(views);
    }

    SECTION("send_zero_copy 空数据") {
        BufferView empty_view;
        auto channel = ChannelFactory::create_client(g_io, "localhost", 0, g_codec);

        // 测试空数据不会崩溃
        channel->send_zero_copy(empty_view);
    }

    SECTION("send_batch_zero_copy 空列表") {
        std::vector<BufferView> empty_views;
        auto channel = ChannelFactory::create_client(g_io, "localhost", 0, g_codec);

        // 测试空列表不会崩溃
        channel->send_batch_zero_copy(empty_views);
    }
}

TEST_CASE("BufferView - 性能验证", "[net][buffer_view]") {
    SECTION("零拷贝: 引用同一数据") {
        ByteBuffer buffer = {0x01, 0x02, 0x03, 0x04, 0x05};

        BufferView view1(buffer);
        BufferView view2(buffer);

        // 验证 view1 和 view2 引用同一数据
        REQUIRE(view1.data() == view2.data());
        REQUIRE(view1.data() == buffer.data());
    }

    SECTION("修改原始数据,视图可见") {
        ByteBuffer buffer = {0x01, 0x02, 0x03};

        BufferView view(buffer);

        // 修改原始数据
        buffer[0] = 0xFF;

        // 视图应该能看到变化
        REQUIRE(view[0] == 0xFF);
    }
}
