#pragma once

#include "AntTask.h"

#include <vector>

namespace TaskAnt {

class AntTask;

class AntEvent {
private:
    // TODO: 使用智能指针销毁
    bool m_finished;
    std::vector<AntTask*> m_subsequents;

public:
    bool Finished();
    void AddSubsequent(AntTask*);
    void DispatchSubsequents();
};

} // namespace TaskAnt
