#include "AntEvent.h"

namespace TaskAnt {

void AntEvent::AddSubsequent(AntTask* pTask)
{
    m_subsequents.push_back(pTask);
}

void AntEvent::DispatchSubsequents()
{
    for (auto task : m_subsequents)
        task->ConditionalQueueTask();
}

} // namespace TaskAnt
