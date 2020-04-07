#pragma once

#include "AntTask.h"

#include <future>
#include <vector>

using namespace std;

namespace TaskAnt {

class AntTask;

class AntEvent {
   private:
    bool m_running;
    bool m_finished;
    std::vector<AntTask*> m_subsequents;  // TODO: Not thread safe
    promise<int> m_finishPromise;
    future<int> m_finishFuture;
    void AddSubsequent(AntTask*);
    void BeforeRun();
    void AfterRun();
    friend class AntTask;
    friend class AntManager;

   public:
    AntEvent();
    bool Running();
    bool Finished();
    void Complete();
};

}  // namespace TaskAnt
