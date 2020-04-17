#include <list>
#include <memory>
#include <string>
#include <vector>

namespace TaskAnt {

class AntEvent;
struct TaskNode;

class AntWatcher {
   private:
    std::list<std::tuple<int, std::vector<int>, std::vector<TaskNode*>>> m_taskStateQueue;

    AntWatcher();

   public:
    static AntWatcher* GetInstance();
    void AddNode(const int& frameNum, const std::string&, const std::shared_ptr<AntEvent>&, const std::vector<std::shared_ptr<AntEvent>>&);
    void ImGuiRenderTick();
};

}  // namespace TaskAnt