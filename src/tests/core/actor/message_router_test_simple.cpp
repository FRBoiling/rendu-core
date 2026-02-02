#include <catch2/catch_test_macros.hpp>
#include <thread>
#include "core/actor/message_router.h"
#include "core/actor/actor_ref.h"
#include "common/io/io_context.h"

using namespace Rendu;

TEST_CASE("Simple MessageRouter test", "[message_router]") {
    Rendu::io::IoContext io(1);
    std::thread t([&io]() { io.run(); });
    t.detach();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    {
        Rendu::MessageRouter router(io, false);
        auto actor1 = Rendu::ActorRef("/system/actor1", 1);
        bool added = router.add_route("route1", [](auto&, auto&) { return true; }, actor1, 10);
        REQUIRE(added);
    }
    
    io.stop();
}
