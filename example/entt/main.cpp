/*
* Created by boil on 2023/3/26.
*/

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include "fake_process.h"


int main() {
    spdlog::set_level(spdlog::level::trace); // or spdlog::set_level(spdlog::level::trace);
    SPDLOG_INFO("------entt begin----------");
    entt::scheduler<int> scheduler;
    fake_process<int> process{};
    bool first_functor = false;
    bool second_functor = false;

    auto attach = [&first_functor](auto, void *, auto resolve, auto) {
        first_functor = true;
        resolve();
        SPDLOG_DEBUG("1");
    };

    auto then = [&second_functor](auto, void *, auto, auto reject) {
        second_functor = true;
        reject();
        SPDLOG_DEBUG("2");
    };

    scheduler.attach(std::move(attach)).then(std::move(then)).then([](auto...) {
        SPDLOG_ERROR("3");
    });

    while(!scheduler.empty()) {
        scheduler.update(0);
    }

    SPDLOG_INFO(first_functor);
    SPDLOG_INFO(second_functor);
    SPDLOG_INFO("-----------entt end----------");
}

