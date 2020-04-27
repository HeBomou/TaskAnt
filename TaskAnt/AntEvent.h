#pragma once

#include <chrono>
#include <future>
#include <mutex>
#include <vector>

#include "AntTask.h"

namespace TaskAnt {

class AntTask;

class AntEvent {
   private:
    std::mutex m_mtx;
    std::vector<AntTask*> m_subsequents;  // TODO: Lockfree
    std::promise<int> m_finishPromise;
    int m_antId = -1;
    long long m_startTimeMicroSecond;
    long long m_runningTimeMicroSecond;
    bool m_finished = false;

    bool TryAddSubsequent(AntTask*);
    void BeforeRun(const int&);
    void AfterRun(std::string name);

    friend class AntManager;
    friend class AntTask;

   public:
    const int& AntId() const;
    const long long& StartTimeMicroSecond() const;
    const long long& RunningTimeMicroSecond() const;
    void Complete();
};

}  // namespace TaskAnt
