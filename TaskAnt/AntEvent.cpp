#include "AntEvent.h"

#include <time.h>

namespace TaskAnt {

void AntEvent::AddSubsequent(AntTask* pTask) { m_subsequents.push_back(pTask); }

void AntEvent::BeforeRun() {
    m_startTime = clock();
}

void AntEvent::AfterRun() {
    m_runningTime = clock() - m_startTime;
    for (auto task : m_subsequents) task->ConditionalQueueTask();
    m_finishPromise.set_value(0);
}

AntEvent::AntEvent() : m_finishPromise(promise<int>()), m_finishFuture(m_finishPromise.get_future()), m_runningTime(0) {}

time_t AntEvent::RunningTime() {
    return m_runningTime;
}

bool AntEvent::Finished() {
    return m_runningTime != 0;
}

void AntEvent::Complete() { m_finishFuture.get(); }

}  // namespace TaskAnt
