#include "AntEvent.h"

#include <chrono>

using namespace std;

namespace TaskAnt {

bool AntEvent::TryAddSubsequent(AntTask* pTask) {
    lock_guard<mutex> lock(m_mtx);
    if (m_finished) return false;
    m_subsequents.push_back(pTask);
    return true;
}

void AntEvent::BeforeRun(const int& antId) {
    m_antId = antId;
    auto startTime = chrono::time_point_cast<chrono::microseconds>(chrono::steady_clock::now());
    m_startTimeMicroSecond = startTime.time_since_epoch().count();
}

void AntEvent::AfterRun(string name) {
    lock_guard<mutex> lock(m_mtx);

    auto runningTime = chrono::time_point_cast<chrono::microseconds>(chrono::steady_clock::now());
    m_runningTimeMicroSecond = runningTime.time_since_epoch().count() - m_startTimeMicroSecond;
    m_finished = true;
    m_finishPromise.set_value(0);
    for (auto task : m_subsequents) task->ConditionalQueueTask();
}

const int& AntEvent::AntId() const {
    return m_antId;
}

const long long& AntEvent::StartTimeMicroSecond() const {
    return m_startTimeMicroSecond;
}

const long long& AntEvent::RunningTimeMicroSecond() const {
    return m_runningTimeMicroSecond;
}

void AntEvent::Complete() { m_finishPromise.get_future().get(); }

}  // namespace TaskAnt
