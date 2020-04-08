#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "Ant.h"
#include "AntEvent.h"
#include "AntTask.h"
#include "AntThread.h"

using namespace std;

namespace TaskAnt {

class AntManager {
   private:
    vector<Ant*> m_pAnts;
    vector<AntThread*> m_pAntThreads;
    condition_variable m_taskQueueCv;
    mutex m_taskQueueMutex;
    queue<AntTask*> m_pTaskQueue;

    AntManager();
    AntTask* GetNextTask();
    void QueueTask(AntTask*);

    friend class Ant;
    friend class AntTask;

   public:
    ~AntManager();
    static AntManager* GetInstance();
    void StartTick();
    shared_ptr<AntEvent> ScheduleTask(AntTask*, vector<shared_ptr<AntEvent>>);
};

}  // namespace TaskAnt
