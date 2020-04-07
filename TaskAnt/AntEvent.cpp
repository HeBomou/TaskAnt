#include "AntEvent.h"

namespace TaskAnt {

AntEvent::AntEvent() {}

void AntEvent::AddSubsequent(AntTask* pTask) { m_subsequents.push_back(pTask); }

void AntEvent::BeforeRun() { m_running = true; }

void AntEvent::AfterRun() {
    m_running = false;
    m_finished = true;
    for (auto task : m_subsequents) task->ConditionalQueueTask();
}

bool AntEvent::Running() { return m_running; }

bool AntEvent::Finished() { return m_finished; }

void AntEvent::Complete() {}

}  // namespace TaskAnt
