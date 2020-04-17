#include "AntManager.h"

#include "AntWatcher.h"

using namespace std;

namespace TaskAnt {

AntManager::AntManager() {
    int antNum = 32;
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

shared_ptr<AntEvent> AntManager::ScheduleTask(const int& frameNum, const string& name, const function<void()>& proc, const vector<shared_ptr<AntEvent>>& pEvents) {
    auto pTask = new AntTask(name, proc);
    auto res = pTask->Setup(pEvents.size() + 1);  // 加一为了锁住任务
    int alreadyFinished = 0;
    for (auto pE : pEvents)
        alreadyFinished += pE->TryAddSubsequent(pTask) ? 0 : 1;
    // TODO: 条件编译
    AntWatcher::GetInstance()->AddNode(frameNum, name, res, pEvents);

    pTask->PrerequisitesComplete(alreadyFinished);
    return res;
}

struct ParallelProcessExecutor {
    int m_begin;
    int m_end;
    function<void(int)> m_proc;
    ParallelProcessExecutor(const int& begin, const int& end, function<void(int)> proc) : m_begin(begin), m_end(end), m_proc(proc) {}
    void operator()() {
        for (int i = m_begin; i < m_end; i++)
            m_proc(i);
    }
};

vector<shared_ptr<AntEvent>> AntManager::ScheduleTaskParallel(const int& frameNum, const string& name, int totalNum, const function<void(int)>& proc, const vector<shared_ptr<AntEvent>>& pEvents) {
    vector<shared_ptr<AntEvent>> res;
    int taskNum = m_pAnts.size();
    int numPerProc = totalNum / taskNum;
    if (numPerProc) {
        numPerProc += (totalNum % taskNum) ? 1 : 0;
        for (int i = 0; i < totalNum; i += numPerProc)
            res.emplace_back(ScheduleTask(frameNum, name, ParallelProcessExecutor(i, min(i + numPerProc, totalNum), proc), pEvents));
    } else {
        for (int i = 0; i < totalNum; i++)
            res.emplace_back(ScheduleTask(frameNum, name, ParallelProcessExecutor(i, i + 1, proc), pEvents));
    }
    return res;
}

}  // namespace TaskAnt
