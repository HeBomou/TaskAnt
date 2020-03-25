#include "AntThread.h"

#include "UniversalPlatformAntThread.h"

namespace TaskAnt {

AntThread::~AntThread() {}

AntThread* AntThread::Create(AntThreadProc* proc)
{
    AntThread* thread = PlatformAntThread::CreatePlatformAntThread();
    thread->CreateInternal(proc);
    return thread;
}

} // namespace TaskAnt
