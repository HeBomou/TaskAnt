#pragma once

#include "AntTask.h"

#include <vector>

namespace TaskAnt {
class AntEvent {
private:
    std::vector<AntTask*> m_subsequents;

public:
    void AddSubsequent(AntTask*);
    void DispatchSubsequents();
};

} // namespace TaskAnt
