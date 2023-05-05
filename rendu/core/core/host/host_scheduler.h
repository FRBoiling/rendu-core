/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_HOST_SCHEDULER_H
#define RENDU_HOST_SCHEDULER_H

#include "host/host.h"
#include "adapter/base/scheduler.h"

namespace rendu {

    class HostScheduler {
        enum class state : std::uint8_t {
            uninitialized = 0,
            running,
            stopping,
            release
        };

    public:
        void Init(Host* host);

        void LaterInit();

        bool IsRunning();

        void Update();

        void LaterUpdate();

        bool IsStopping();

        void Stopping();

        void Release();

        void Exit();

    private:
        state m_current_state{state::uninitialized};
        Host* m_host{};
    };

}


#endif //RENDU_HOST_SCHEDULER_H
