#pragma once

#include "Ant.h"
#include "AntEvent.h"
#include "AntTask.h"
#include "AntThread.h"

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

namespace TaskAnt {

class AntManager {
private:
    std::vector<Ant*> m_pAnts;
    std::vector<AntThread*> m_pAntThreads;
    std::condition_variable m_taskQueueCv; // TODO: Try to impl a thread safe queue or monitor
    std::mutex m_taskQueueMutex;
    std::queue<AntTask*> m_pTaskQueue;

    AntManager();

public:
    ~AntManager();
    static AntManager* GetInstance();
    std::shared_ptr<AntEvent> ScheduleTask(AntTask*, std::vector<std::shared_ptr<AntEvent>>);
    AntTask* GetNextTask();
    void QueueTask(AntTask*);
};

} // namespace TaskAnt
