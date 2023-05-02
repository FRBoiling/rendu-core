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


