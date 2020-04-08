#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>

#include <chrono>
#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "AntWatcher/AntWatcher.h"
#include "ImNodesEz.h"
#include "TaskAnt/AntEvent.h"
#include "TaskAnt/AntManager.h"
#include "TaskAnt/AntTask.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

struct TestTask : public TaskAnt::AntTask {
    int m_outputNum;
    int m_time;
    TestTask(string name, const int& outputNum) : AntTask(name), m_outputNum(outputNum) {}
    virtual ~TestTask() override {}
    virtual void Run() override {
        for (int i = 0; i < m_outputNum; i++)
            m_time++, this_thread::sleep_for(chrono::milliseconds(100));
    }
};

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

GLFWwindow* InitContext() {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return NULL;

    // Decide GL+GLSL versions
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac

    // Create window with graphics context
    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Task Ant Test", NULL, NULL);
    if (window == NULL) return NULL;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Initialize OpenGL loader
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return NULL;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard
    // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable
    // Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return window;
}

void ScheduleTestTasks() {
    // 启动若干任务
    auto event1 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 1", 20), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event2 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 14), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event3 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 13), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event4 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 12), vector<shared_ptr<TaskAnt::AntEvent>>{event3});
    auto event5 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 8), vector<shared_ptr<TaskAnt::AntEvent>>{event1, event2});
    auto event6 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 3), vector<shared_ptr<TaskAnt::AntEvent>>{event2});
    auto event7 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 5), vector<shared_ptr<TaskAnt::AntEvent>>{event5});
    auto event8 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 9), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event9 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 7), vector<shared_ptr<TaskAnt::AntEvent>>{event4, event8});
    auto event10 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 9), vector<shared_ptr<TaskAnt::AntEvent>>{event6});
    auto event11 = TaskAnt::AntManager::GetInstance()->ScheduleTask(new TestTask("Task 2", 9), vector<shared_ptr<TaskAnt::AntEvent>>{event10, event9});
}

int main() {
    GLFWwindow* window = InitContext();
    if (!window) return 1;

    // State
    ImVec4 clear_color = ImColor(204, 234, 244);

    float tick = 0.3f;
    float timer = 0;
    time_t preTime = clock();

    ScheduleTestTasks();

    while (!glfwWindowShouldClose(window)) {
        // game logic
        time_t curTime = clock();
        timer += curTime - preTime;
        preTime = curTime;
        if (timer >= tick) {
            // Fixed update
            timer -= tick;
        }

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 渲染依赖图
        AntWatcher::GetInstance()->ImGuiRenderTick();

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

    return 0;
}