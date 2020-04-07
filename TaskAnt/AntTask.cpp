#include "AntTask.h"

#include "AntManager.h"

namespace TaskAnt {

AntTask::~AntTask() {}

void AntTask::SetInDegree(int inDegree) { m_inDegree = inDegree; }

void AntTask::ConditionalQueueTask() {
    if (--m_inDegree == 0) {
        AntManager::GetInstance()->QueueTask(this);
    }
}

void AntTask::BeforeRun() { m_event->BeforeRun(); }

void AntTask::AfterRun() {
    m_event->AfterRun();
}

std::shared_ptr<AntEvent> AntTask::InitEvent() {
    return m_event = std::make_shared<AntEvent>();
}

}  // namespace TaskAnt
