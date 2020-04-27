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
    int m_antId = -1;
    time_t m_startTime = -1;
    time_t m_runningTime = 0;
    bool m_finished = false;

    bool TryAddSubsequent(AntTask *);
    void BeforeRun(const int&);
    void AfterRun(std::string name);

    friend class AntManager;
    friend class AntTask;

   public:
    const int& AntId() const;
    const time_t& StartTime() const;
    const time_t& RunningTime() const;
    void Complete();
};

}  // namespace TaskAnt
