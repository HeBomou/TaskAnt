#pragma once

#include "AntThreadProc.h"

namespace TaskAnt {

class AntThread {
   protected:
    AntThreadProc* m_pProc;
    virtual void CreateInternal(AntThreadProc*) = 0;

   public:
    virtual ~AntThread() = 0;
    static AntThread* Create(AntThreadProc*);
    virtual void Join() = 0;
};

}  // namespace TaskAnt
