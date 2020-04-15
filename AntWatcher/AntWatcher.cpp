#include "AntWatcher.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../game.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

AntWatcher::AntWatcher() {
}

AntWatcher* AntWatcher::GetInstance() {
    static AntWatcher instance;
    return &instance;
}

void AntWatcher::AddNode(const string& taskName, const shared_ptr<TaskAnt::AntEvent>& event, const vector<shared_ptr<TaskAnt::AntEvent>>& deps) {
    // TODO: 无等待，但是AddNode只能在主线程
    if (m_taskNodeQueue.back().first != g_frameNum)
        m_taskNodeQueue.emplace_back(make_pair(g_frameNum, vector<TaskNode*>()));
    auto& taskNodes = m_taskNodeQueue.back().second;
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
    if (m_nodeNumInCols.size() < col)
        m_nodeNumInCols.emplace_back(0);
    int row = ++m_nodeNumInCols[col - 1];
    newNode->m_col = col;
    newNode->m_pos = ImVec2(col * intervalX - intervalX * 0.7, row * intervalY + !(col & 1) * intervalY * 0.5);
}

void AntWatcher::ImGuiRenderTick() {
    static ImNodes::CanvasState canvas{};
    static vector<ImNodes::Ez::SlotInfo> inputSlots{{"Deps", 1}};
    static vector<ImNodes::Ez::SlotInfo> outputSlots{{"Event", 1}};

    ImNodes::BeginCanvas(&canvas);

    // 无等待，其他线程在队尾加，渲染线程画队首
    if (m_taskNodeQueue.size() >= 4)
        for (auto it = m_taskNodeQueue.front().second.begin(); it != m_taskNodeQueue.front().second.end();) {
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
    while (m_taskNodeQueue.size() > 4) m_taskNodeQueue.pop_front();

    ImNodes::EndCanvas();
}
