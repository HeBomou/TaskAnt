#pragma once

#include "AntEvent.h"

#include <memory>

namespace TaskAnt {

class AntEvent;

class AntTask {
private:
    int m_inDegree = 0;
    std::shared_ptr<AntEvent> m_event;
public:
    virtual ~AntTask() = 0;
    void SetInDegree(int);
    void ConditionalQueueTask();
    void BeforeRun();
    virtual void Run() = 0;
    std::shared_ptr<AntEvent> InitEvent();
    void NotifySubsequents();
};

} // namespace TaskAnt
