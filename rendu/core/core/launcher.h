/*
* Created by boil on 2023/2/16.
*/

#ifndef RENDU_CORE_LAUNCHER_H_
#define RENDU_CORE_LAUNCHER_H_

#include "base/host_scheduler.h"
#include "base/host_process.h"

namespace rendu {

    class Launcher {
    private:
        Host m_host;
        HostScheduler m_scheduler;
    public:
        template<typename T>
        Host &AddPlugin() {
            return m_host.AddPlugin<T>();
        }

        void Run() {
            m_scheduler.attach<HostProcess>(&m_host);
            while (!m_scheduler.empty()) {
                m_scheduler.update(0);
            }
            m_scheduler.abort();
        }
    };

}
#endif //RENDU_CORE_LAUNCHER_H_
