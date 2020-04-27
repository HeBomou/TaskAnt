#include "AntTask.h"

#include "AntManager.h"

using namespace std;

namespace TaskAnt {

shared_ptr<AntEvent> AntTask::Setup(const int& inDegree) {
    m_inDegree = inDegree;
    return m_event = make_shared<AntEvent>();
}

void AntTask::PrerequisitesComplete(const int& alreadyFinished) {
    if ((m_inDegree -= (alreadyFinished + 1)) == 0)
        AntManager::GetInstance()->QueueTask(this);
}

void AntTask::ConditionalQueueTask() {
    if (--m_inDegree == 0)
        AntManager::GetInstance()->QueueTask(this);
}

void AntTask::BeforeRun(const int& antId) { m_event->BeforeRun(antId); }

void AntTask::AfterRun() { m_event->AfterRun(m_name); }

AntTask::AntTask(const string& name, const function<void()>& proc) : m_name(name), m_proc(proc) {}

}  // namespace TaskAnt
