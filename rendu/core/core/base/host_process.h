/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_HOST_PROCESS_H
#define RENDU_HOST_PROCESS_H

#include "common/base/process.h"
#include "host.h"

namespace rendu {

    class HostProcess : public Process<HostProcess, std::uint32_t> {
    public:
        using process_type = Process<HostProcess, std::uint32_t>;
        using delta_type = typename process_type::delta_type;

        explicit HostProcess(Host* host);

        void init();

        void succeeded();

        void failed();

        void aborted();

        void update(typename Process<HostProcess, std::uint32_t>::delta_type, void *data);

    private:
        Host* m_host;
    };

}

#endif //RENDU_HOST_PROCESS_H
