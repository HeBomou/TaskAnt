#include "TaskAnt/AntEvent.h"
#include "TaskAnt/AntManager.h"
#include "TaskAnt/AntTask.h"

#include <cstdio>
#include <vector>
#include <chrono>
#include <thread>

class TestProc : public TaskAnt::AntTask {
public:
    virtual ~TestProc() override {}
    virtual void Run() override
    {
        std::printf("Hello ant Task!\n");
    }
};

int main()
{
    TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc(), std::vector<TaskAnt::AntEvent*>());
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    return 0;
}