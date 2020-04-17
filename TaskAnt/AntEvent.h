#pragma once

#include <future>
#include <mutex>
#include <vector>

#include "AntTask.h"

namespace TaskAnt {

class AntTask;

class AntEvent {
   private:
    std::mutex m_mtx;
    std::vector<AntTask *> m_subsequents;  // TODO: Lockfree
    std::promise<int> m_finishPromise;
    time_t m_startTime;
    time_t m_runningTime = 0;
    bool m_finished = false;

    bool TryAddSubsequent(AntTask *);
    void BeforeRun();
    void AfterRun(std::string name);

    friend class AntManager;
    friend class AntTask;

   public:
    time_t RunningTime();
    void Complete();
};

}  // namespace TaskAnt
