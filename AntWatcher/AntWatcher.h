#include "../TaskAnt/AntEvent.h"
#include "../TaskAnt/AntManager.h"
#include "../TaskAnt/AntTask.h"

#include <ImNodesEz.h>

#include <memory>
#include <vector>

struct TaskNode;

// 边
struct Connection {
    // `id` that was passed to BeginNode() of input node.
    TaskNode* input_node = nullptr;
    // `id` that was passed to BeginNode() of output node.
    TaskNode* output_node = nullptr;

    bool operator==(const Connection& other) const
    {
        return input_node == other.input_node && output_node == other.output_node;
    }

    bool operator!=(const Connection& other) const
    {
        return !operator==(other);
    }
};

struct TaskNode {
    const char* m_title;
    bool m_selected = false;
    ImVec2 m_pos{};
    std::vector<Connection> m_deps{};
    // 事件
    std::shared_ptr<TaskAnt::AntEvent> m_event;

    explicit TaskNode(const char* title,
        const std::shared_ptr<TaskAnt::AntEvent>& event)
        : m_title(title)
        , m_event(event)
    {
    }

    // Deletes connection from this node.
    void DeleteConnection(const Connection& connection)
    {
        for (auto it = m_deps.begin(); it != m_deps.end(); ++it) {
            if (connection == *it) {
                m_deps.erase(it);
                break;
            }
        }
    }
};

class AntWatcher {

private:
    std::vector<TaskNode*> m_taskNodes;
    AntWatcher();

public:
    static AntWatcher* GetInstance();
    void AddNode(const char*, const std::shared_ptr<TaskAnt::AntEvent>&, const std::vector<std::shared_ptr<TaskAnt::AntEvent>>&);
    void ImGuiRenderTick();
};