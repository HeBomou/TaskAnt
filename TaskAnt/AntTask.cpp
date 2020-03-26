#include "AntTask.h"

#include "AntManager.h"

namespace TaskAnt {

AntTask::~AntTask() {}

void AntTask::SetInDegree(int inDegree)
{
    m_inDegree = inDegree;
}

void AntTask::ConditionalQueueTask()
{
    if (--m_inDegree == 0) {
        AntManager::GetInstance()->QueueTask(this);
    }
}

AntEvent* AntTask::InitEvent()
{
    return m_event = new AntEvent();
}

void AntTask::NotifySubsequents()
{
    m_event->DispatchSubsequents();
}

} // namespace TaskAnt
