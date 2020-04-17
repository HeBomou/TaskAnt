#include "Ant.h"

#include "AntManager.h"

// #include <chrono>
// #include <thread>

namespace TaskAnt {

Ant::Ant() {}

Ant::~Ant() {}

void Ant::Run() {
    while (!m_killed) {
        auto pTask = AntManager::GetInstance()->GetNextTask();
        if (pTask) {
            pTask->BeforeRun();
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
