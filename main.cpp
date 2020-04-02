#include "TaskAnt/AntEvent.h"
#include "TaskAnt/AntManager.h"
#include "TaskAnt/AntTask.h"

#include <chrono>
#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ImNodesEz.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct TestTask : public TaskAnt::AntTask {
    int& m_outputNum;
    bool& m_running;
    int& m_time;
    TestTask(int& outputNum, bool& running, int& time)
        : m_outputNum(outputNum)
        , m_running(running)
        , m_time(time)
    {
    }
    virtual ~TestTask() override
    {
    }
    virtual void Run() override
    {
        m_running = true;
        for (int i = 0; i < m_outputNum; i++)
            m_time++, std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        m_running = false;
    }
};

/// A structure defining a connection between two slots of two nodes.
struct Connection {
    /// `id` that was passed to BeginNode() of input node.
    void* input_node = nullptr;
    /// Descriptor of input slot.
    const char* input_slot = nullptr;
    /// `id` that was passed to BeginNode() of output node.
    void* output_node = nullptr;
    /// Descriptor of output slot.
    const char* output_slot = nullptr;

    bool operator==(const Connection& other) const
    {
        return input_node == other.input_node && input_slot == other.input_slot && output_node == other.output_node && output_slot == other.output_slot;
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
    std::vector<Connection> m_connections{};
    std::vector<ImNodes::Ez::SlotInfo> m_input_slots{};
    std::vector<ImNodes::Ez::SlotInfo> m_output_slots{};
    // 任务
    TestTask* m_task;
    int m_outputNum = 5;
    bool m_running = false;
    int m_time = 0;

    explicit TaskNode(const char* title,
        const std::vector<ImNodes::Ez::SlotInfo>&& input_slots,
        const std::vector<ImNodes::Ez::SlotInfo>&& output_slots)
        : m_title(title)
        , m_input_slots(input_slots)
        , m_output_slots(output_slots)
        , m_task(new TestTask(m_outputNum, m_running, m_time))
    {
    }

    /// Deletes connection from this node.
    void DeleteConnection(const Connection& connection)
    {
        for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
            if (connection == *it) {
                m_connections.erase(it);
                break;
            }
        }
    }
};

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    // TODO: delete 键

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // State
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Task
    // auto eventA = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('A', 3, runningA, timeA), std::vector<std::shared_ptr<TaskAnt::AntEvent>>());
    // auto eventB = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('B', 6, runningB, timeB), std::vector<std::shared_ptr<TaskAnt::AntEvent>>());
    // auto eventC = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('C', 5, runningC, timeC), std::vector<std::shared_ptr<TaskAnt::AntEvent>>{ eventA });
    // auto eventD = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('D', 4, runningD, timeD), std::vector<std::shared_ptr<TaskAnt::AntEvent>>{ eventB, eventC });

    std::vector<TaskNode*> nodes;

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 启动任务
        if (ImGui::Button("Run tasks"))
            for (auto node : nodes)
                TaskAnt::AntManager::GetInstance()->ScheduleTask(node->m_task, std::vector<std::shared_ptr<TaskAnt::AntEvent>>());

        // Canvas must be created after ImGui initializes, because constructor accesses ImGui style to configure default colors.
        static ImNodes::CanvasState canvas{};
        // const ImGuiStyle& style = ImGui::GetStyle();

        ImNodes::BeginCanvas(&canvas);
        for (auto it = nodes.begin(); it != nodes.end();) {
            auto node = *it;
            // Start rendering node
            if (ImNodes::Ez::BeginNode(node, node->m_title, &node->m_pos, &node->m_selected)) {
                // Render input nodes first (order is important)
                ImNodes::Ez::InputSlots(node->m_input_slots.data(), node->m_input_slots.size());

                // Custom node content may go here
                ImGui::PushItemWidth(200);
                ImGui::Text("Time to run");
                ImGui::InputInt("", &node->m_outputNum);
                ImGui::Text("State: %s", node->m_running ? "Running" : "Stopped");
                ImGui::Text("Counter: %d", node->m_time);

                // Render output nodes first (order is important)
                ImNodes::Ez::OutputSlots(node->m_output_slots.data(), node->m_output_slots.size());

                // Store new connections when they are created
                Connection new_connection;
                if (ImNodes::GetNewConnection(&new_connection.input_node, &new_connection.input_slot,
                        &new_connection.output_node, &new_connection.output_slot)) {
                    ((TaskNode*)new_connection.input_node)->m_connections.push_back(new_connection);
                    ((TaskNode*)new_connection.output_node)->m_connections.push_back(new_connection);
                }

                // Render output connections of this node
                for (const Connection& connection : node->m_connections) {
                    // Node contains all it's connections (both from output and to input slots). This means that multiple
                    // nodes will have same connection. We render only output connections and ensure that each connection
                    // will be rendered once.
                    if (connection.output_node != node)
                        continue;

                    if (!ImNodes::Connection(connection.input_node, connection.input_slot, connection.output_node,
                            connection.output_slot)) {
                        // Remove deleted connections
                        ((TaskNode*)connection.input_node)->DeleteConnection(connection);
                        ((TaskNode*)connection.output_node)->DeleteConnection(connection);
                    }
                }
                // Node rendering is done. This call will render node background based on size of content inside node.
                ImNodes::Ez::EndNode();

                // 删除节点
                if (node->m_selected && ImGui::IsKeyPressedMap(ImGuiKey_Delete)) {
                    // Deletion order is critical: first we delete connections to us
                    for (auto& connection : node->m_connections) {
                        if (connection.output_node == node) {
                            ((TaskNode*)connection.input_node)->DeleteConnection(connection);
                        } else {
                            ((TaskNode*)connection.output_node)->DeleteConnection(connection);
                        }
                    }
                    // Then we delete our own connections, so we don't corrupt the list
                    node->m_connections.clear();

                    delete node;
                    it = nodes.erase(it);
                } else
                    ++it;
            }
        }

        // 右键菜单
        if (ImGui::IsMouseReleased(1) && ImGui::IsWindowHovered() && !ImGui::IsMouseDragging(1)) {
            ImGui::FocusWindow(ImGui::GetCurrentWindow());
            ImGui::OpenPopup("NodesContextMenu");
        }
        if (ImGui::BeginPopup("NodesContextMenu")) {
            // 新增任务
            if (ImGui::MenuItem("New task")) {
                nodes.push_back(new TaskNode("Task", { { "Deps", 1 } }, { { "Event", 1 } }));
                ImNodes::AutoPositionNode(nodes.back());
            }
            ImGui::Separator();
            // 恢复缩放
            if (ImGui::MenuItem("Reset Zoom"))
                canvas.zoom = 1;
            // 关闭菜单
            if (ImGui::IsAnyMouseDown() && !ImGui::IsWindowHovered())
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImNodes::EndCanvas();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    getchar();
    return 0;
}