#pragma once

#include "AntThread.h"

#include <thread>

namespace TaskAnt {

class UniversalPlatformAntThread : public AntThread {
private:
    std::thread m_thread;
    static void ThreadEntryPoint(UniversalPlatformAntThread* pThis);

protected:
    virtual void CreateInternal(AntThreadProc*) override;

public:
    virtual ~UniversalPlatformAntThread() override;
    static AntThread* CreatePlatformAntThread();
    virtual void Join() override;
};

typedef UniversalPlatformAntThread PlatformAntThread;

} // namespace TaskAnt
