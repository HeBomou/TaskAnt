#pragma once

#include "AntTask.h"

#include <future>
#include <vector>

using namespace std;

namespace TaskAnt {

class AntTask;

class AntEvent {
   private:
    vector<AntTask*> m_subsequents;  // TODO: Not thread safe
    promise<int> m_finishPromise;
    future<int> m_finishFuture;
    time_t m_startTime;
    time_t m_runningTime;

    void AddSubsequent(AntTask*);
    void BeforeRun();
    void AfterRun();

    friend class AntManager;
    friend class AntTask;

   public:
    AntEvent();
    time_t RunningTime();
    bool Finished();
    void Complete();
};

}  // namespace TaskAnt
