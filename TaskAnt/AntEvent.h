#pragma once

#include "AntTask.h"

#include <vector>

namespace TaskAnt {

class AntTask;

class AntEvent {
private:
    bool m_running;
    bool m_finished;
    std::vector<AntTask*> m_subsequents;

public:
    void MarkRunning();
    bool Running();
    bool Finished();
    void AddSubsequent(AntTask*);
    void DispatchSubsequents();
};

} // namespace TaskAnt
