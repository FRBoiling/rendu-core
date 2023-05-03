/*
* Created by boil on 2023/5/2.
*/

#include "host_process.h"
#include "host.h"

using namespace rendu;

HostProcess::HostProcess(Host *host) : m_host(host) {

}

void HostProcess::update(Process<HostProcess, std::uint32_t>::delta_type delta, void *data) {
    RD_INFO("host process update : {}", delta);
    auto systems = m_host->GetSystems(SystemType::Update);
    for(const auto& iter:systems){
        auto& system = iter.second;
        auto& system_key =iter.first;
        system->Run(m_host->m_entity_pool);
    }
}

void HostProcess::init() {
    RD_INFO("host process init");
}

void HostProcess::succeeded() {
    RD_INFO("host process succeeded");
}

void HostProcess::failed() {
    RD_INFO("host process failed");
}

void HostProcess::aborted() {
    RD_INFO("host process aborted");
}


