#pragma once

namespace TaskAnt {

class AntTask {
private:
    int m_inDegree = 0;
public:
    virtual ~AntTask() = 0;
    void SetInDegree(int);
    void ConditionalQueueTask();
    virtual void Run() = 0;
};

} // namespace TaskAnt
