#pragma once

#include "AntThreadProc.h"

namespace TaskAnt {

class Ant : public AntThreadProc {
   private:
    bool m_killed = false;
    int m_id;

   public:
    explicit Ant(const int& id);
    ~Ant();
    virtual void Run() override;
    virtual void Stop() override;
};

}  // namespace TaskAnt
