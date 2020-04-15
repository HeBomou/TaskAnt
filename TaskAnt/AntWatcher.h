#include <list>
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace TaskAnt {

class AntEvent;
struct TaskNode;

class AntWatcher {
   private:
    list<tuple<int, vector<int>, vector<TaskNode*>>> m_taskStateQueue;

    AntWatcher();

   public:
    static AntWatcher* GetInstance();
    void AddNode(const string&, const shared_ptr<AntEvent>&, const vector<shared_ptr<AntEvent>>&);
    void ImGuiRenderTick();
};

}  // namespace TaskAnt