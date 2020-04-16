#pragma once

#include <future>
#include <mutex>
#include <vector>

#include "AntTask.h"

using namespace std;

namespace TaskAnt {

class AntTask;

class AntEvent {
   private:
    mutex m_mtx;
    vector<AntTask *> m_subsequents; // TODO: Lockfree
    promise<int> m_finishPromise;
    time_t m_startTime;
    time_t m_runningTime = 0;
    bool m_finished = false;

    bool TryAddSubsequent(AntTask *);
    void BeforeRun();
    void AfterRun(string name);

    friend class AntManager;
    friend class AntTask;

   public:
    time_t RunningTime();
    void Complete();
};

}  // namespace TaskAnt
