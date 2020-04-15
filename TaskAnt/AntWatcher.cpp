#include "AntWatcher.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ImNodesEz.h>
#include <imgui.h>

#include "AntEvent.h"
#include "AntManager.h"

namespace TaskAnt {

struct TaskNode;

// 边
struct Connection {
    // `id` that was passed to BeginNode() of input node.
    TaskNode* input_node = nullptr;
    // `id` that was passed to BeginNode() of output node.
    TaskNode* output_node = nullptr;

    bool operator==(const Connection& other) const {
        return input_node == other.input_node && output_node == other.output_node;
    }

    bool operator!=(const Connection& other) const {
        return !operator==(other);
    }
};

struct TaskNode {
    string m_title;
    bool m_selected = false;
    int m_col;
    ImVec2 m_pos{};
    vector<Connection> m_deps{};
    // 事件
    shared_ptr<AntEvent> m_event;

    explicit TaskNode(const string& title, const shared_ptr<AntEvent>& event) : m_title(title), m_event(event) {
    }

    // Deletes connection from this node.
    void DeleteConnection(const Connection& connection) {
        for (auto it = m_deps.begin(); it != m_deps.end(); ++it) {
            if (connection == *it) {
                m_deps.erase(it);
                break;
            }
        }
    }
};

AntWatcher::AntWatcher() {
}

AntWatcher* AntWatcher::GetInstance() {
    static AntWatcher instance;
    return &instance;
}

void AntWatcher::AddNode(const int& frameNum, const string& taskName, const shared_ptr<AntEvent>& event, const vector<shared_ptr<AntEvent>>& deps) {
    // TODO: 无等待，但是AddNode只能在主线程
    if (get<0>(m_taskStateQueue.back()) != frameNum)
        m_taskStateQueue.emplace_back(make_tuple(frameNum, vector<int>(), vector<TaskNode*>()));
    auto& taskNodes = get<2>(m_taskStateQueue.back());
    auto& nodeNumInCols = get<1>(m_taskStateQueue.back());
    const int intervalX = 350;
    const int intervalY = 100;
    auto newNode = new TaskNode(taskName, event);
    int col = 1;
    taskNodes.push_back(newNode);
    for (auto dep : deps)
        for (auto node : taskNodes) {
            if (node->m_event == dep) {
                Connection c;
                c.input_node = newNode, c.output_node = node;
                newNode->m_deps.push_back(c);
                col = max(col, node->m_col + 1);
                break;
            }
        }
    if (nodeNumInCols.size() < col)
        nodeNumInCols.emplace_back(0);
    int row = ++nodeNumInCols[col - 1];
    newNode->m_col = col;
    newNode->m_pos = ImVec2(col * intervalX - intervalX * 0.7, row * intervalY + !(col & 1) * intervalY * 0.5);
}

void AntWatcher::ImGuiRenderTick() {
    static ImNodes::CanvasState canvas{};
    static vector<ImNodes::Ez::SlotInfo> inputSlots{{"Deps", 1}};
    static vector<ImNodes::Ez::SlotInfo> outputSlots{{"Event", 1}};

    ImNodes::BeginCanvas(&canvas);

    // 无等待，其他线程在队尾加，渲染线程画队首
    const int maxSize = 4;
    if (m_taskStateQueue.size() >= maxSize) {
        for (auto it = get<2>(m_taskStateQueue.front()).begin(); it != get<2>(m_taskStateQueue.front()).end();) {
            auto node = *it;
            if (ImNodes::Ez::BeginNode(node, node->m_title.c_str(), &node->m_pos, &node->m_selected)) {
                // 输入插槽
                ImNodes::Ez::InputSlots(inputSlots.data(), inputSlots.size());

                // 任务信息
                ImGui::PushItemWidth(40);
                time_t milliTime = node->m_event->RunningTime();
                float time = (float)milliTime / CLOCKS_PER_SEC;
                ImGui::TextColored(ImColor(0, 240, 0), "Running time: %.4f", time);

                // 输出插槽
                ImNodes::Ez::OutputSlots(outputSlots.data(), outputSlots.size());

                // 绘制边
                for (const Connection& connection : node->m_deps) {
                    if (!ImNodes::Connection(connection.input_node, "Deps", connection.output_node,
                                             "Event")) {
                        // Remove deleted connections
                        connection.input_node->DeleteConnection(connection);
                    }
                }

                ImNodes::Ez::EndNode();
            }
            it++;
        }
        // 让渲染固定慢几帧
        while (m_taskStateQueue.size() > maxSize) m_taskStateQueue.pop_front();
    }

    ImNodes::EndCanvas();
}

}  // namespace TaskAnt