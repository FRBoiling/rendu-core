/*
* Created by boil on 2023/5/2.
*/

#include "host_scheduler.h"

using namespace rendu;

void HostScheduler::Init(Host *host) {
    m_host = host;
    RD_INFO("HostScheduler Init success");
    auto& systems = m_host->GetSystems(SystemType::Init);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
}

void HostScheduler::LaterInit() {
    m_current_state = state::running;
    RD_INFO("HostScheduler LaterInit success");
    auto& systems = m_host->GetSystems(SystemType::LateInit);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
}

bool HostScheduler::IsRunning() {
    return state::running == m_current_state;
}

void HostScheduler::Update() {
    auto& systems = m_host->GetSystems(SystemType::Update);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
}

void HostScheduler::LaterUpdate() {
    auto& systems = m_host->GetSystems(SystemType::LateUpdate);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
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

