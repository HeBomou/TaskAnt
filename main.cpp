#include "TaskAnt/AntEvent.h"
#include "TaskAnt/AntManager.h"
#include "TaskAnt/AntTask.h"

#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ImNodesEz.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class TestProc : public TaskAnt::AntTask {
private:
    char m_label;
    int m_outputNum;
    bool& m_show;
    int& m_time;

public:
    TestProc(char label, int outputNum, bool& show, int& time)
        : m_label(label)
        , m_outputNum(outputNum)
        , m_show(show)
        , m_time(time)
    {
    }
    virtual ~TestProc() override
    {
    }
    virtual void Run() override
    {
        m_show = true;
        for (int i = 0; i < m_outputNum; i++)
            m_time++, std::printf("Test Proc %c!\n", m_label), std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        m_show = false;
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

/// A structure holding node state.
struct TaskNode {
    /// Title which will be displayed at the center-top of the node.
    const char* title = nullptr;
    /// Flag indicating that node is selected by the user.
    bool selected = false;
    /// Node position on the canvas.
    ImVec2 pos{};
    /// List of node connections.
    std::vector<Connection> connections{};
    /// A list of input slots current node has.
    std::vector<ImNodes::Ez::SlotInfo> input_slots{};
    /// A list of output slots current node has.
    std::vector<ImNodes::Ez::SlotInfo> output_slots{};

    explicit TaskNode(const char* title,
        const std::vector<ImNodes::Ez::SlotInfo>&& input_slots,
        const std::vector<ImNodes::Ez::SlotInfo>&& output_slots)
    {
        this->title = title;
        this->input_slots = input_slots;
        this->output_slots = output_slots;
    }

    /// Deletes connection from this node.
    void DeleteConnection(const Connection& connection)
    {
        for (auto it = connections.begin(); it != connections.end(); ++it) {
            if (connection == *it) {
                connections.erase(it);
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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // State
    bool runningA = false, runningB = false, runningC = false, runningD = false;
    int timeA = 0, timeB = 0, timeC = 0, timeD = 0;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Task
    auto eventA = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('A', 3, runningA, timeA), std::vector<std::shared_ptr<TaskAnt::AntEvent>>());
    auto eventB = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('B', 6, runningB, timeB), std::vector<std::shared_ptr<TaskAnt::AntEvent>>());
    auto eventC = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('C', 5, runningC, timeC), std::vector<std::shared_ptr<TaskAnt::AntEvent>>{ eventA });
    auto eventD = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestProc('D', 4, runningD, timeD), std::vector<std::shared_ptr<TaskAnt::AntEvent>>{ eventB, eventC });

    std::vector<TaskNode*> nodes = { new TaskNode("He", {}, {}) };

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Text("HelloWorld %d", 123);

        // Canvas must be created after ImGui initializes, because constructor accesses ImGui style to configure default colors.
        static ImNodes::CanvasState canvas{};
        // const ImGuiStyle& style = ImGui::GetStyle();

        ImNodes::BeginCanvas(&canvas);
        for (auto node : nodes) {
            // Start rendering node
            if (ImNodes::Ez::BeginNode(node, node->title, &node->pos, &node->selected)) {
                // Render input nodes first (order is important)
                ImNodes::Ez::InputSlots(node->input_slots.data(), node->input_slots.size());

                // Custom node content may go here
                ImGui::Text("Content of %s", node->title);

                // Render output nodes first (order is important)
                ImNodes::Ez::OutputSlots(node->output_slots.data(), node->output_slots.size());

                // Store new connections when they are created
                Connection new_connection;
                if (ImNodes::GetNewConnection(&new_connection.input_node, &new_connection.input_slot,
                        &new_connection.output_node, &new_connection.output_slot)) {
                    ((TaskNode*)new_connection.input_node)->connections.push_back(new_connection);
                    ((TaskNode*)new_connection.output_node)->connections.push_back(new_connection);
                }

                // Render output connections of this node
                for (const Connection& connection : node->connections) {
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
            }
        }
        ImNodes::EndCanvas();

        // ImGui::Begin("Task A");
        // ImGui::Text("Time: %d", timeA);
        // ImGui::Text(runningA ? "Running" : "Stopped");
        // ImGui::End();

        // ImGui::Begin("Task B");
        // ImGui::Text("Time: %d", timeB);
        // ImGui::Text(runningB ? "Running" : "Stopped");
        // ImGui::End();

        // ImGui::Begin("Task C");
        // ImGui::Text("Time: %d", timeC);
        // ImGui::Text(runningC ? "Running" : "Stopped");
        // ImGui::End();

        // ImGui::Begin("Task D");
        // ImGui::Text("Time: %d", timeD);
        // ImGui::Text(runningD ? "Running" : "Stopped");
        // ImGui::End();

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