/*
* Created by boil on 2023/4/27.
*/

#ifndef RENDU_FAKE_PROCESS_H
#define RENDU_FAKE_PROCESS_H

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

struct fake_delta {};

template<typename Delta>
struct fake_process: entt::process<fake_process<Delta>, Delta> {
    using process_type = entt::process<fake_process<Delta>, Delta>;
    using delta_type = typename process_type::delta_type;

    fake_process()
            : init_invoked{false},
              update_invoked{false},
              succeeded_invoked{false},
              failed_invoked{false},
              aborted_invoked{false} {}

    void succeed() noexcept {
        SPDLOG_INFO("succeed");
        process_type::succeed();
    }

    void fail() noexcept {
        SPDLOG_INFO("fail");
        process_type::fail();
    }

    void pause() noexcept {
        SPDLOG_INFO("pause");
        process_type::pause();
    }

    void unpause() noexcept {
        SPDLOG_INFO("unpause");
        process_type::unpause();
    }

    void init() {
        SPDLOG_INFO("init");
        init_invoked = true;
    }

    void succeeded() {
        SPDLOG_INFO("succeeded");
        succeeded_invoked = true;
    }

    void failed() {
        SPDLOG_INFO("failed");
        failed_invoked = true;
    }

    void aborted() {
        SPDLOG_INFO("aborted");
        aborted_invoked = true;
    }

    void update(typename entt::process<fake_process<Delta>, Delta>::delta_type, void *data) {
        if(data) {
            (*static_cast<int *>(data))++;
        }
        SPDLOG_INFO("update");
        update_invoked = true;
    }

    bool init_invoked;
    bool update_invoked;
    bool succeeded_invoked;
    bool failed_invoked;
    bool aborted_invoked;
};


#endif //RENDU_FAKE_PROCESS_H
