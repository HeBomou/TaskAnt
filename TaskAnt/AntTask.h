#pragma once

#include "AntEvent.h"

namespace TaskAnt {

class AntEvent;

class AntTask {
private:
    int m_inDegree = 0;
    AntEvent *m_event;
public:
    virtual ~AntTask() = 0;
    void SetInDegree(int);
    void ConditionalQueueTask();
    virtual void Run() = 0;
    AntEvent* InitEvent();
    void NotifySubsequents();
};

} // namespace TaskAnt
