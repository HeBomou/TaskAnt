#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "Ant.h"
#include "AntEvent.h"
#include "AntTask.h"
#include "AntThread.h"

namespace TaskAnt {

class AntManager {
   private:
    std::vector<Ant*> m_pAnts;
    std::vector<AntThread*> m_pAntThreads;
    std::condition_variable m_taskQueueCv;
    std::mutex m_taskQueueMutex;
    std::queue<AntTask*> m_pTaskQueue;

    AntManager();
    AntTask* GetNextTask();
    void QueueTask(AntTask*);

    friend class Ant;
    friend class AntTask;

   public:
    ~AntManager();
    static AntManager* GetInstance();
    std::shared_ptr<AntEvent> ScheduleTask(const int&, const std::string&, const std::function<void()>&, const std::vector<std::shared_ptr<AntEvent>>&);
    std::vector<std::shared_ptr<AntEvent>> ScheduleTaskParallel(const int&, const std::string&, int, const std::function<void(int)>&, const std::vector<std::shared_ptr<AntEvent>>&);
};

}  // namespace TaskAnt
