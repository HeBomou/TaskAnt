#pragma once

#include <memory>
#include <atomic>
#include <string>

#include "AntEvent.h"

namespace TaskAnt {

class AntEvent;

class AntTask {
   private:
    std::string m_name;
    std::function<void()> m_proc;
    std::atomic<int> m_inDegree;
    std::shared_ptr<AntEvent> m_event;

    std::shared_ptr<AntEvent> Setup(int);
    void PrerequisitesComplete(int);
    void ConditionalQueueTask();
    void BeforeRun();
    void AfterRun();

    friend class AntManager;
    friend class AntEvent;
    friend class Ant;

   public:
    AntTask(const std::string& name, const std::function<void()>& proc);
};

}  // namespace TaskAnt
