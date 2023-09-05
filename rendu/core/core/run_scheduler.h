/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_RUN_SCHEDULER_H
#define RENDU_RUN_SCHEDULER_H

#include "host/host.h"

namespace rendu {

    class RunScheduler {
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


#endif //RENDU_RUN_SCHEDULER_H
