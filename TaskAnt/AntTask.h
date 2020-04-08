#pragma once

#include <memory>
#include <string>

#include "AntEvent.h"

using namespace std;

namespace TaskAnt {

class AntEvent;

class AntTask {
   private:
    string m_name;
    int m_inDegree = 0;
    shared_ptr<AntEvent> m_event;
    void SetInDegree(int);
    shared_ptr<AntEvent> InitEvent();
    void ConditionalQueueTask();
    void BeforeRun();
    void AfterRun();
    friend class AntManager;
    friend class AntEvent;
    friend class Ant;

   protected:
    virtual void Run() = 0;

   public:
    AntTask(const string& name);
    virtual ~AntTask() = 0;
    const string& GetName() const;
};

}  // namespace TaskAnt
