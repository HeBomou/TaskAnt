#include "AntEvent.h"

#include <ctime>

namespace TaskAnt {

bool AntEvent::TryAddSubsequent(AntTask* pTask) {
    lock_guard<mutex> lock(m_mtx);
    if (m_finished) return false;
    m_subsequents.push_back(pTask);
    return true;
}

void AntEvent::BeforeRun() {
    m_startTime = clock();
}

void AntEvent::AfterRun(string name) {
    lock_guard<mutex> lock(m_mtx);
    m_runningTime = clock() - m_startTime;
    m_finished = true;
    m_finishPromise.set_value(0);
    for (auto task : m_subsequents) task->ConditionalQueueTask();
}

time_t AntEvent::RunningTime() {
    return m_runningTime;
}

void AntEvent::Complete() { m_finishPromise.get_future().get(); }

}  // namespace TaskAnt
