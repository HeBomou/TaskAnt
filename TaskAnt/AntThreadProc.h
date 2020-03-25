#pragma once

namespace TaskAnt {

class AntThreadProc {
private:
public:
    virtual ~AntThreadProc() = 0;
    virtual void Run() = 0;
    virtual void Stop() = 0;
};

} // namespace TaskAnt
