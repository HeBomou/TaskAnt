#pragma once

#include "AntTask.h"

#include <vector>

namespace TaskAnt {

class AntTask;

class AntEvent {
   private:
    bool m_running;
    bool m_finished;
    std::vector<AntTask*> m_subsequents;  // TODO: Not thread safe
    AntEvent();
    void AddSubsequent(AntTask*);
    void BeforeRun();
    void AfterRun();
    friend class AntTask;
    friend class AntManager;

   public:
    bool Running();
    bool Finished();
    void Complete();
};

}  // namespace TaskAnt
