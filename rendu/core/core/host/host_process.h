/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_HOST_PROCESS_H
#define RENDU_HOST_PROCESS_H

#include "adapter/fwd/adapter_fwd.h"
#include "host.h"

namespace rendu {

    class HostProcess : public Process<HostProcess, std::uint32_t> {

    public:
        using process_type = Process<HostProcess, std::uint32_t>;
        using delta_type = typename process_type::delta_type;

        explicit HostProcess(Host *host);


    private:
        Host *m_host;
    };

}

#endif //RENDU_HOST_PROCESS_H
