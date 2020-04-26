#include <list>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace TaskAnt {

class AntEvent;
struct TaskNode;

class AntWatcher {
   private:
    std::mutex m_mtx;
    std::list<std::tuple<int, std::vector<int>, std::vector<TaskNode*>>> m_taskStateQueue;
    int m_tasksToDisplayFrameNum = -1;
    std::vector<TaskNode*> m_tasksToDisplay;
    bool m_pause = 0;

    AntWatcher();

   public:
    static AntWatcher* GetInstance();
    void AddNode(const int& frameNum, const std::string&, const std::shared_ptr<AntEvent>&, const std::vector<std::shared_ptr<AntEvent>>&);
    void ImGuiRenderTick();
};

}  // namespace TaskAnt