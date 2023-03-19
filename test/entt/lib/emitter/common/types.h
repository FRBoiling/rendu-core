#ifndef ENTT_LIB_EMITTER_COMMON_TYPES_H
#define ENTT_LIB_EMITTER_COMMON_TYPES_H

#include <entt/signal/emitter.hpp>
#define PLUGIN "～/Repos/rendu/bin/libproto_model.a"

struct test_emitter
    : entt::emitter<test_emitter> {};

struct message {
    int payload;
};

struct event {};

#endif
