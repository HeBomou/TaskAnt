#include "AntManager.h"

// #include <chrono>
// #include <thread>

namespace TaskAnt {

AntManager::AntManager()
{
    int antNum = 8;
    for (int i = 0; i < antNum; i++) {
        auto pAnt = new Ant();
        auto pAntThread = AntThread::Create(pAnt);
        m_pAnts.push_back(pAnt);
        m_pAntThreads.push_back(pAntThread);
    }
}

AntManager::~AntManager()
{
    // TODO: 目前在退出时是放弃未执行的任务，可以考虑改成完成所有的任务
    // while (!m_pTaskQueue.empty()) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }
    for (auto pAnt : m_pAnts)
        pAnt->Stop();
    for (auto pThread : m_pAntThreads)
        delete pThread;
    while (!m_pTaskQueue.empty()) {
        auto pTask = m_pTaskQueue.front();
        m_pTaskQueue.pop();
        delete pTask;
    }
}

AntManager* AntManager::GetInstance()
{
    static AntManager instance;
    return &instance;
}

AntEvent* AntManager::ScheduleTask(AntTask* pTask, std::vector<AntEvent*> pEvents)
{
    int inDegree = pEvents.size();
    for (auto pE : pEvents) {
        if (!pE->Finished())
            pE->AddSubsequent(pTask);
        else
            inDegree--;
    }
    pTask->SetInDegree(inDegree);
    if (inDegree == 0)
        AntManager::GetInstance()->QueueTask(pTask);
    return pTask->InitEvent();
}

AntTask* AntManager::GetNextTask()
{
    std::lock_guard<std::mutex> lock(m_taskQueueMutex);
    if (m_pTaskQueue.empty())
        return nullptr;
    auto res = m_pTaskQueue.front();
    m_pTaskQueue.pop();
    return res;
}

void AntManager::QueueTask(AntTask* pTask)
{
    std::lock_guard<std::mutex> lock(m_taskQueueMutex);
    m_pTaskQueue.push(pTask);
}

} // namespace TaskAnt
