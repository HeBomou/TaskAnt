#include "AntEvent.h"

namespace TaskAnt {

void AntEvent::AddSubsequent(AntTask* pTask) { m_subsequents.push_back(pTask); }

void AntEvent::BeforeRun() { m_running = true; }

void AntEvent::AfterRun() {
    m_running = false;
    m_finished = true;
    for (auto task : m_subsequents) task->ConditionalQueueTask();
    m_finishPromise.set_value(0);
}

AntEvent::AntEvent()
    : m_finishPromise(promise<int>()),
      m_finishFuture(m_finishPromise.get_future()) {}

bool AntEvent::Running() { return m_running; }

bool AntEvent::Finished() { return m_finished; }

void AntEvent::Complete() { m_finishFuture.get(); }

}  // namespace TaskAnt
