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

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Text("HelloWorld %d", 123);

        ImGui::Begin("Task A");
        ImGui::Text("Time: %d", timeA);
        ImGui::Text(runningA ? "Running" : "Stopped");
        ImGui::End();

        ImGui::Begin("Task B");
        ImGui::Text("Time: %d", timeB);
        ImGui::Text(runningB ? "Running" : "Stopped");
        ImGui::End();

        ImGui::Begin("Task C");
        ImGui::Text("Time: %d", timeC);
        ImGui::Text(runningC ? "Running" : "Stopped");
        ImGui::End();

        ImGui::Begin("Task D");
        ImGui::Text("Time: %d", timeD);
        ImGui::Text(runningD ? "Running" : "Stopped");
        ImGui::End();

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