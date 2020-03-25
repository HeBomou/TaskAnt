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

} // namespace TaskAnt
