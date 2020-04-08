#pragma once

#include "Ant.h"
#include "AntEvent.h"
#include "AntTask.h"
#include "AntThread.h"

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

using namespace std;

namespace TaskAnt {

class AntManager {
   private:
    vector<Ant*> m_pAnts;
    vector<AntThread*> m_pAntThreads;
    condition_variable m_taskQueueCv;
    mutex m_taskQueueMutex;
    queue<AntTask*> m_pTaskQueue;
    AntTask* GetNextTask();
    void QueueTask(AntTask*);
    friend class Ant;
    friend class AntTask;

    AntManager();

   public:
    ~AntManager();
    static AntManager* GetInstance();
    shared_ptr<AntEvent> ScheduleTask(AntTask*, vector<shared_ptr<AntEvent>>);
};

}  // namespace TaskAnt
