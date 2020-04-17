#pragma once

#include <memory>
#include <atomic>
#include <string>

#include "AntEvent.h"

using namespace std;

namespace TaskAnt {

class AntEvent;

class AntTask {
   private:
    string m_name;
    function<void()> m_proc;
    atomic<int> m_inDegree;
    shared_ptr<AntEvent> m_event;

    shared_ptr<AntEvent> Setup(int);
    void PrerequisitesComplete(int);
    void ConditionalQueueTask();
    void BeforeRun();
    void AfterRun();

    friend class AntManager;
    friend class AntEvent;
    friend class Ant;

   public:
    AntTask(const string& name, const function<void()>& proc);
};

}  // namespace TaskAnt
