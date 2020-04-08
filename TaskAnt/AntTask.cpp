#include "AntTask.h"

#include "AntManager.h"

namespace TaskAnt {

void AntTask::SetInDegree(int inDegree) { m_inDegree = inDegree; }

shared_ptr<AntEvent> AntTask::InitEvent() {
    return m_event = make_shared<AntEvent>();
}

void AntTask::ConditionalQueueTask() {
    if (--m_inDegree == 0)
        AntManager::GetInstance()->QueueTask(this);
}

void AntTask::BeforeRun() { m_event->BeforeRun(); }

void AntTask::AfterRun() { m_event->AfterRun(); }

AntTask::AntTask(string name) : m_name(name) {}

AntTask::~AntTask() {}

const string& AntTask::GetName() const { return m_name; }

}  // namespace TaskAnt
