#pragma once

#include "AntThreadProc.h"

namespace TaskAnt {

class Ant : public AntThreadProc {
   private:
    bool m_killed = false;

   public:
    Ant();
    ~Ant();
    virtual void Run() override;
    virtual void Stop() override;
};

}  // namespace TaskAnt
