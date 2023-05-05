/*
* Created by boil on 2023/5/2.
*/

#include "host_scheduler.h"

using namespace rendu;

void HostScheduler::Init(Host *host) {
    m_host = host;
    RD_INFO("HostScheduler Init success");
}

void HostScheduler::LaterInit() {
    m_current_state = state::running;
    RD_INFO("HostScheduler LaterInit success");
}

bool HostScheduler::IsRunning() {
    return state::running == m_current_state;
}

void HostScheduler::Update() {

}

void HostScheduler::LaterUpdate() {
    
}

bool HostScheduler::IsStopping() {
    return state::stopping == m_current_state;
}

void HostScheduler::Stopping() {
    if (!IsStopping()){
        return;
    }
    RD_INFO("HostScheduler Stopping...");
}

void HostScheduler::Release() {
    RD_INFO("HostScheduler Release...");
    m_current_state = state::release;
    RD_INFO("HostScheduler Exit...");
}

void HostScheduler::Exit() {
    m_current_state = state::stopping;
}

