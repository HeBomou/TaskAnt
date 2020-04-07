#pragma once

#include "AntEvent.h"

#include <memory>

using namespace std;

namespace TaskAnt {

class AntEvent;

class AntTask {
   private:
    int m_inDegree = 0;
    shared_ptr<AntEvent> m_event;

   public:
    virtual ~AntTask() = 0;
    void SetInDegree(int);
    void ConditionalQueueTask();
    void BeforeRun();
    virtual void Run() = 0;
    void AfterRun();
    shared_ptr<AntEvent> InitEvent();
};

}  // namespace TaskAnt
