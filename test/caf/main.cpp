/*
* Created by boil on 2023/3/27.
*/
#define CAF_SUITE ping_pong

#include <test/rdtest.h>
#include "caf/all.hpp"

using namespace caf;

// --(rst-ping-pong-begin)--
namespace {

behavior ping(event_based_actor* self, actor pong_actor, int n) {
  self->send(pong_actor, ping_atom_v, n);
  return {
      [=](pong_atom, int x) {
        if (x > 1)
          self->send(pong_actor, ping_atom_v, x - 1);
      },
  };
}

behavior pong() {
  return {
      [=](ping_atom, int x) { return make_result(pong_atom_v, x); },
  };
}

} // namespace


TEST(CAF,ThreePings) {
// Spawn the Ping actor and run its initialization code.
//auto ping_actor = sys.spawn(ping, pong_actor, 3);
//sched.run_once();
//// Test communication between Ping and Pong.
//  EXPECT_EQ((ping_atom, int), from(ping_actor).to(pong_actor).with(_, 3));
//  EXPECT_EQ((pong_atom, int), from(pong_actor).to(ping_actor).with(_, 3));
//  EXPECT_EQ((ping_atom, int), from(ping_actor).to(pong_actor).with(_, 2));
//  EXPECT_EQ((pong_atom, int), from(pong_actor).to(ping_actor).with(_, 2));
//  EXPECT_EQ((ping_atom, int), from(ping_actor).to(pong_actor).with(_, 1));
//  EXPECT_EQ((pong_atom, int), from(pong_actor).to(ping_actor).with(_, 1));
//// No further messages allowed.
//disallow((ping_atom, int), from(ping_actor).to(pong_actor).with(_, 1));
}



