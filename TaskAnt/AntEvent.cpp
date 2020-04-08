#include "AntEvent.h"

#include <time.h>

namespace TaskAnt {

bool AntEvent::TryAddSubsequent(AntTask* pTask) {
    lock_guard<mutex> lock(m_mtx);
    if (m_runningTime) return false;
    printf("Try add %s\n", pTask->m_name.c_str());
    m_subsequents.push_back(pTask);
    return true;
}

void AntEvent::BeforeRun() {
    m_startTime = clock();
}

void AntEvent::AfterRun(string name) {
    lock_guard<mutex> lock(m_mtx);
    printf("After run %s\n", name.c_str());
    m_runningTime = clock() - m_startTime;
    m_finishPromise.set_value(0);
    for (auto task : m_subsequents) task->ConditionalQueueTask();
}

time_t AntEvent::RunningTime() {
    return m_runningTime;
}

void AntEvent::Complete() { m_finishPromise.get_future().get(); }

}  // namespace TaskAnt
