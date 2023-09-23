/*
* Created by boil on 2023/5/2.
*/

#include "run_scheduler.h"

using namespace rendu;

void RunScheduler::Init(Host *host) {
    m_host = host;
    RD_INFO("RunScheduler Init success");
    auto& systems = m_host->GetSystems(SystemType::Init);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
}

void RunScheduler::LaterInit() {
    m_current_state = state::running;
    RD_INFO("RunScheduler LaterInit success");
    auto& systems = m_host->GetSystems(SystemType::LateInit);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
}

bool RunScheduler::IsRunning() {
    return state::running == m_current_state;
}

void RunScheduler::Update() {
    auto& systems = m_host->GetSystems(SystemType::Update);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
}

void RunScheduler::LaterUpdate() {
    auto& systems = m_host->GetSystems(SystemType::LateUpdate);
    for(auto & system : systems){
        system->Run(m_host->m_entity_pool);
    }
}

bool RunScheduler::IsStopping() {
    return state::stopping == m_current_state;
}

void RunScheduler::Stopping() {
    if (!IsStopping()){
        return;
    }
    RD_INFO("RunScheduler Stopping...");
}

void RunScheduler::Release() {
    RD_INFO("RunScheduler Release...");
    m_current_state = state::release;
    RD_INFO("RunScheduler Exit...");
}

void RunScheduler::Exit() {
    m_current_state = state::stopping;
}

