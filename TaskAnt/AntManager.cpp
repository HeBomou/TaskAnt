#include "AntManager.h"

#include "../AntWatcher/AntWatcher.h"

namespace TaskAnt {

AntManager::AntManager() {
    int antNum = 4;
    for (int i = 0; i < antNum; i++) {
        auto pAnt = new Ant();
        auto pAntThread = AntThread::Create(pAnt);
        m_pAnts.push_back(pAnt);
        m_pAntThreads.push_back(pAntThread);
    }
}

AntTask* AntManager::GetNextTask() {
    unique_lock<mutex> lock(m_taskQueueMutex);
    if (m_pTaskQueue.empty()) m_taskQueueCv.wait(lock);
    if (m_pTaskQueue.empty()) return nullptr;
    auto res = m_pTaskQueue.front();
    m_pTaskQueue.pop();
    return res;
}

void AntManager::QueueTask(AntTask* pTask) {
    unique_lock<mutex> lock(m_taskQueueMutex);
    m_pTaskQueue.push(pTask);
    m_taskQueueCv.notify_one();
}

AntManager::~AntManager() {
    // TODO: 目前在退出时是放弃未执行的任务，可以考虑改成完成所有的任务
    for (auto pAnt : m_pAnts) pAnt->Stop();
    m_taskQueueCv.notify_all();
    for (auto pThread : m_pAntThreads) delete pThread;
    while (!m_pTaskQueue.empty()) {
        auto pTask = m_pTaskQueue.front();
        m_pTaskQueue.pop();
        delete pTask;
    }
}

AntManager* AntManager::GetInstance() {
    static AntManager instance;
    return &instance;
}

void AntManager::StartTick() {
    AntWatcher::GetInstance()->Clean();
}

shared_ptr<AntEvent> AntManager::ScheduleTask(AntTask* pTask, vector<shared_ptr<AntEvent>> pEvents) {
    int inDegree = pEvents.size();
    for (auto pE : pEvents) {
        if (!pE->Finished())
            pE->AddSubsequent(pTask);
        else
            inDegree--;
    }
    pTask->SetInDegree(inDegree);
    auto res = pTask->InitEvent();
    AntWatcher::GetInstance()->AddNode(pTask->GetName(), res, pEvents);
    if (inDegree == 0) AntManager::GetInstance()->QueueTask(pTask);
    return res;
}

}  // namespace TaskAnt
