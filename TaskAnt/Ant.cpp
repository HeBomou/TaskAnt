#include "Ant.h"

#include "AntManager.h"

// #include <chrono>
// #include <thread>

namespace TaskAnt {

Ant::Ant(const int& id) : m_id(id) {}

Ant::~Ant() {}

void Ant::Run() {
    while (!m_killed) {
        auto pTask = AntManager::GetInstance()->GetNextTask();
        if (pTask) {
            pTask->BeforeRun(m_id);
            pTask->m_proc();
            pTask->AfterRun();
            delete pTask;
        }
    }
}

void Ant::Stop() {
    m_killed = true;
}

}  // namespace TaskAnt
