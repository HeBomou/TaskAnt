#include "UniversalPlatformAntThread.h"

namespace TaskAnt {

void UniversalPlatformAntThread::CreateInternal(AntThreadProc* pProc)
{
    m_pProc = pProc;
    m_thread = std::thread(ThreadEntryPoint, this);
}

void UniversalPlatformAntThread::ThreadEntryPoint(UniversalPlatformAntThread* pThis)
{
    pThis->m_pProc->Run();
}

UniversalPlatformAntThread::~UniversalPlatformAntThread()
{
    Join();
    delete m_pProc;
}

AntThread* UniversalPlatformAntThread::CreatePlatformAntThread()
{
    return new UniversalPlatformAntThread();
}

void UniversalPlatformAntThread::Join()
{
    m_thread.join();
}

} // namespace TaskAnt
