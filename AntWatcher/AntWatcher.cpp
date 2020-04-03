#include "AntWatcher.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

AntWatcher::AntWatcher()
{
}

AntWatcher* AntWatcher::GetInstance()
{
    static AntWatcher instance;
    return &instance;
}

void AntWatcher::AddNode(const char* taskName, const std::shared_ptr<TaskAnt::AntEvent>& event, const std::vector<std::shared_ptr<TaskAnt::AntEvent>>& deps)
{
    auto newNode = new TaskNode(taskName, event);
    newNode->m_pos = ImVec2(arc4random() % 500, arc4random() % 500);
    m_taskNodes.push_back(newNode);
    for (auto dep : deps)
        for (auto node : m_taskNodes) {
            if (node->m_event == dep) {
                Connection c;
                c.input_node = newNode, c.output_node = node;
                newNode->m_deps.push_back(c);
                break;
            }
        }
}

void AntWatcher::ImGuiRenderTick()
{
    static ImNodes::CanvasState canvas{};
    static std::vector<ImNodes::Ez::SlotInfo> inputSlots{ { "Deps", 1 } };
    static std::vector<ImNodes::Ez::SlotInfo> outputSlots{ { "Event", 1 } };

    ImNodes::BeginCanvas(&canvas);

    for (auto it = m_taskNodes.begin(); it != m_taskNodes.end();) {
        auto node = *it;
        if (ImNodes::Ez::BeginNode(node, node->m_title, &node->m_pos, &node->m_selected)) {
            // 输入插槽
            ImNodes::Ez::InputSlots(inputSlots.data(), inputSlots.size());

            // 任务信息
            ImGui::PushItemWidth(40);
            bool running = node->m_event->Running();
            bool finished = node->m_event->Finished();
            ImGui::TextColored(finished ? ImColor(150, 150, 150) : running ? ImColor(0, 240, 0) : ImColor(240, 0, 0), finished ? "Finished" : running ? "Running" : "Waiting");

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

    ImNodes::EndCanvas();
}
