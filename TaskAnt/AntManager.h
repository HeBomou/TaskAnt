#pragma once

#include "Ant.h"
#include "AntEvent.h"
#include "AntTask.h"
#include "AntThread.h"

#include <mutex>
#include <queue>
#include <vector>

namespace TaskAnt {

class AntManager {
private:
    std::vector<Ant*> m_pAnts;
    std::vector<AntThread*> m_pAntThreads;
    std::mutex m_taskQueueMutex; // TODO: Try to impl a thread safe queue
    std::queue<AntTask*> m_pTaskQueue;

    AntManager();

public:
    ~AntManager();
    static AntManager* GetInstance();
    void ScheduleTask(AntTask*, std::vector<AntEvent*>);
    AntTask* GetNextTask();
    void QueueTask(AntTask*);
};

} // namespace TaskAnt
