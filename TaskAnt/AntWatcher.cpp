#include "AntWatcher.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imnodes.h>

#include <map>

#include "AntEvent.h"
#include "AntManager.h"

namespace TaskAnt {

struct TaskNode;

// 边
struct Connection {
    const int m_id;
    const TaskNode* m_dep = nullptr;

    Connection(const int& id, const TaskNode* dep) : m_id(id), m_dep(dep) {}
};

struct TaskNode {
   private:
    static int s_idCnt;
    static map<string, tuple<int, int, int>> s_nodeMap;
    static map<string, int> s_edgeMap;
    vector<Connection> m_deps{};

   public:
    int m_id;
    int m_inputId;
    int m_outputId;
    string m_title;
    bool m_selected = false;
    int m_col;
    ImVec2 m_pos{};
    // 事件
    shared_ptr<AntEvent> m_event;

    explicit TaskNode(const string& title, const shared_ptr<AntEvent>& event) : m_title(title), m_event(event) {
        if (!s_nodeMap.count(title))
            s_nodeMap.insert(make_pair(title, make_tuple(s_idCnt + 1, s_idCnt + 2, s_idCnt + 3))), s_idCnt += 3;
        auto ids = s_nodeMap[title];
        m_id = get<0>(ids);
        m_inputId = get<1>(ids);
        m_outputId = get<2>(ids);
    }

    void AddDep(const TaskNode* dep) {
        string eStr = m_title + dep->m_title;
        if (!s_edgeMap.count(eStr))
            s_edgeMap.insert(make_pair(eStr, ++s_idCnt));
        m_deps.push_back(Connection(s_edgeMap[eStr], dep));
    }

    const vector<Connection>& GetDeps() {
        return m_deps;
    }
};
int TaskNode::s_idCnt = 0;
map<string, tuple<int, int, int>> TaskNode::s_nodeMap{};
map<string, int> TaskNode::s_edgeMap{};

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
    const int intervalX = 150;
    const int intervalY = 100;
    auto newNode = new TaskNode(taskName, event);
    int col = 1;
    taskNodes.push_back(newNode);
    for (auto dep : deps)
        for (auto node : taskNodes) {
            if (node->m_event == dep) {
                newNode->AddDep(node);
                col = max(col, node->m_col + 1);
                break;
            }
        }
    if (nodeNumInCols.size() < col)
        nodeNumInCols.emplace_back(0);
    int row = ++nodeNumInCols[col - 1];
    newNode->m_col = col;
    newNode->m_pos = ImVec2(col * intervalX - intervalX * 0.8, row * intervalY + !(col & 1) * intervalY * 0.3 - intervalY * 0.8);
}

void AntWatcher::ImGuiRenderTick() {
    ImGui::Begin("Dependencies");

    imnodes::BeginNodeEditor();

    // 无等待，其他线程在队尾加，渲染线程画队首
    const int maxSize = 4;
    if (m_taskStateQueue.size() >= maxSize) {
        for (auto it = get<2>(m_taskStateQueue.front()).begin(); it != get<2>(m_taskStateQueue.front()).end();) {
            auto node = *it;
            imnodes::BeginNode(node->m_id);
            // 标题
            imnodes::BeginNodeTitleBar();
            ImGui::Text("%s", node->m_title.c_str());
            imnodes::EndNodeTitleBar();
            // 运行时间
            ImGui::TextColored(ImColor(0, 240, 0), "%.4f", (float)node->m_event->RunningTime() / CLOCKS_PER_SEC);
            // 输入插槽
            imnodes::BeginInputAttribute(node->m_inputId);
            ImGui::Text("Dep");
            imnodes::EndAttribute();
            // 输出插槽
            imnodes::BeginOutputAttribute(node->m_outputId);
            ImGui::Text("Event");
            imnodes::EndAttribute();
            imnodes::EndNode();
            // 位置
            imnodes::SetNodeGridSpacePos(node->m_id, node->m_pos);
            for (const Connection& connection : node->GetDeps())
                imnodes::Link(connection.m_id, node->m_inputId, connection.m_dep->m_outputId);
            it++;
        }
        // 让渲染固定慢几帧
        while (m_taskStateQueue.size() > maxSize) m_taskStateQueue.pop_front();
    }

    imnodes::EndNodeEditor();

    ImGui::End();
}

}  // namespace TaskAnt