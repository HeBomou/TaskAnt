#include "Ant.h"

#include "AntManager.h"

// #include <chrono>
// #include <thread>

namespace TaskAnt {

Ant::Ant() {}

Ant::~Ant() {}

void Ant::Run()
{
    while (!m_killed) {
        auto pTask = AntManager::GetInstance()->GetNextTask();
        if (pTask) {
            pTask->Run();
            pTask->NotifySubsequents();
            delete pTask;
        }// else std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

void Ant::Stop()
{
    m_killed = true;
}

} // namespace TaskAnt
