#include "TaskAnt/AntEvent.h"
#include "TaskAnt/AntManager.h"
#include "TaskAnt/AntTask.h"

#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

class TestProc : public TaskAnt::AntTask {
private:
    char m_label;
    int m_outputNum;

public:
    TestProc(char label, int outputNum)
        : m_label(label)
        , m_outputNum(outputNum)
    {
    }
    virtual ~TestProc() override
    {
    }
    virtual void Run() override
    {
        for (int i = 0; i < m_outputNum; i++)
            std::printf("Test Proc %c!\n", m_label), std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
};

int main()
{
    auto eventA = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('A', 3), std::vector<std::shared_ptr<TaskAnt::AntEvent>>());
    auto eventB = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('B', 6), std::vector<std::shared_ptr<TaskAnt::AntEvent>>());
    auto eventC = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('C', 5), std::vector<std::shared_ptr<TaskAnt::AntEvent>>{ eventA });
    auto eventD = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('D', 4), std::vector<std::shared_ptr<TaskAnt::AntEvent>>{ eventB, eventC });
    getchar();
    return 0;
}