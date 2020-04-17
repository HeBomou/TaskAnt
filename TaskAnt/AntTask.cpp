#include "AntTask.h"

#include "AntManager.h"

namespace TaskAnt {

shared_ptr<AntEvent> AntTask::Setup(int inDegree) {
    m_inDegree = inDegree;
    return m_event = make_shared<AntEvent>();
}

void AntTask::PrerequisitesComplete(int alreadyFinished) {
    if ((m_inDegree -= (alreadyFinished + 1)) == 0)
        AntManager::GetInstance()->QueueTask(this);
}

void AntTask::ConditionalQueueTask() {
    if (--m_inDegree == 0)
        AntManager::GetInstance()->QueueTask(this);
}

void AntTask::BeforeRun() { m_event->BeforeRun(); }

void AntTask::AfterRun() { m_event->AfterRun(m_name); }

AntTask::AntTask(const string& name, function<void()> proc) : m_name(name), m_proc(proc) {}

}  // namespace TaskAnt
