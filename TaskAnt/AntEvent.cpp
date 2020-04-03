#include "AntEvent.h"

namespace TaskAnt {

void AntEvent::MarkRunning()
{
    m_running = true;
}

bool AntEvent::Running()
{
    return m_running;
}

bool AntEvent::Finished()
{
    return m_finished;
}

void AntEvent::AddSubsequent(AntTask* pTask)
{
    m_subsequents.push_back(pTask);
}

void AntEvent::DispatchSubsequents()
{
    m_running = false;
    m_finished = true;
    for (auto task : m_subsequents)
        task->ConditionalQueueTask();
}

} // namespace TaskAnt
