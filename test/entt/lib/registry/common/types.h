#ifndef ENTT_LIB_REGISTRY_COMMON_TYPES_H
#define ENTT_LIB_REGISTRY_COMMON_TYPES_H

#define PLUGIN "～/Repos/rendu/bin/libproto_model.a"
template
class entt::basic_registry<entt::entity>;
struct position {
    int x;
    int y;
};

struct velocity {
    double dx;
    double dy;
};

#endif
