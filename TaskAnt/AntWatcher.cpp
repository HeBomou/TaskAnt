#include "AntWatcher.h"

#include <imgui.h>
#include <imnodes.h>

#include <algorithm>
#include <map>

#include "AntEvent.h"
#include "AntManager.h"

using namespace std;

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

void AntWatcher::LogTick() {
    // TODO: 输出到日志文件
}

void AntWatcher::ImGuiRenderTick() {
    ImGui::Begin("Controller");
    // 暂停按钮
    if (m_pause) {
        if (ImGui::Button("Resume"))
            m_pause = false;
    } else if (ImGui::Button("Pause"))
        m_pause = true;
    ImGui::End();

    // 无等待，其他线程在队尾加，渲染线程画队首
    const int maxSize = 4;
    if (m_taskStateQueue.size() >= maxSize) {
        if (!m_pause && m_tasksToDisplayFrameNum != get<0>(m_taskStateQueue.front()))
            m_tasksToDisplay = get<2>(m_taskStateQueue.front());
        // 根据AntId排序把相同线程的任务排在一起
        sort(m_tasksToDisplay.begin(), m_tasksToDisplay.end(), [](TaskNode* a, TaskNode* b) { return a->m_event->AntId() < b->m_event->AntId(); });
        // 让渲染固定慢几帧
        while (m_taskStateQueue.size() > maxSize) m_taskStateQueue.pop_front();
    } else
        return;

    // 显示依赖关系
    ImGui::Begin("Dependencies");
    // ImGui::SetWindowSize(ImVec2(600, 400));
    imnodes::BeginNodeEditor();
    for (auto it = m_tasksToDisplay.begin(); it != m_tasksToDisplay.end(); it++) {
        auto node = *it;
        imnodes::BeginNode(node->m_id);
        // 标题
        imnodes::BeginNodeTitleBar();
        ImGui::Text("%s", node->m_title.c_str());
        imnodes::EndNodeTitleBar();
        // 运行时间
        ImGui::TextColored(ImColor(0, 240, 0), "%.4f", (float)node->m_event->RunningTime() * 1000 / CLOCKS_PER_SEC);
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
    }
    imnodes::EndNodeEditor();
    ImGui::End();

    // XXX: 显示时间线
    ImGui::Begin("Timeline");

    int reserveT = 100;
    int lineHeight = 40;
    float factor = 0.5f;
    time_t minT = m_tasksToDisplay.empty() ? -1 : m_tasksToDisplay.front()->m_event->StartTime();
    for (auto node : m_tasksToDisplay)
        minT = min(minT, node->m_event->StartTime());

    int preAntId = -1;

    for (auto node : m_tasksToDisplay) {
        if (node->m_event->AntId() != preAntId) {
            preAntId = node->m_event->AntId();
            ImGui::Button(("Ant " + to_string(preAntId)).c_str(), ImVec2(reserveT >> 1, lineHeight));
        }
        const string& taskName = node->m_title;
        float startTime = (float)(node->m_event->StartTime() - minT) * 1000000 / CLOCKS_PER_SEC * factor;
        float runningTime = (float)node->m_event->RunningTime() * 1000000 / CLOCKS_PER_SEC * factor + 0.01f;
        ImGui::SameLine(startTime + reserveT);
        ImGui::Button(taskName.c_str(), ImVec2(runningTime, lineHeight));
    }

    ImGui::End();
}

AntWatcher* AntWatcher::GetInstance() {
    static AntWatcher instance;
    return &instance;
}

void AntWatcher::AddNode(const int& frameNum, const string& taskName, const shared_ptr<AntEvent>& event, const vector<shared_ptr<AntEvent>>& deps) {
    // AddNode可以在任意线程
    lock_guard<mutex> lock(m_mtx);
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

void AntWatcher::Tick() {
    ImGuiRenderTick();
}

}  // namespace TaskAnt