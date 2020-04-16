#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "TaskAnt/AntEvent.h"
#include "TaskAnt/AntManager.h"
#include "TaskAnt/AntTask.h"
#include "TaskAnt/AntWatcher.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imnodes.h"

using namespace std;

struct TestTask : public TaskAnt::AntTask {
    int m_outputNum;
    int m_time;
    TestTask(string name, const int& outputNum) : AntTask(name), m_outputNum(outputNum) {}
    virtual ~TestTask() override {}
    virtual void Run() override {
        for (int i = 0; i < m_outputNum; i++) {
            m_time++;
            for (int j = 0; j < m_outputNum * 1000; j++)
                m_time += sqrt(j);
        }
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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Task Ant Test", NULL, NULL);
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

    imnodes::Initialize();

    return window;
}

void Cleanup(GLFWwindow* window) {
    imnodes::Shutdown();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void ScheduleAndFinishTestTasks(int frameNum) {
    // 启动若干任务
    auto event1 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 1", 2), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event2 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 2", 4), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event3 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 3", 3), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event4 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 4", 2), vector<shared_ptr<TaskAnt::AntEvent>>{event3});
    auto event5 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 5", 8), vector<shared_ptr<TaskAnt::AntEvent>>{event1, event2});
    auto event6 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 6", 3), vector<shared_ptr<TaskAnt::AntEvent>>{event2});
    auto event7 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 7", 5), vector<shared_ptr<TaskAnt::AntEvent>>{event5});
    auto event8 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 8", 9), vector<shared_ptr<TaskAnt::AntEvent>>{});
    auto event9 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 9", 7), vector<shared_ptr<TaskAnt::AntEvent>>{event4, event8});
    auto event10 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 10", 9), vector<shared_ptr<TaskAnt::AntEvent>>{event6});
    auto event11 = TaskAnt::AntManager::GetInstance()->ScheduleTask(frameNum, new TestTask("Task 11", 9), vector<shared_ptr<TaskAnt::AntEvent>>{event4, event5, event6, event7, event9, event10});

    // 完成任务
    event11->Complete();
}

void GameLoopProc() {
    long long tick = 0.02 * CLOCKS_PER_SEC;
    long long timer = 0;
    long long preTime = clock();

    int frameNum = 0;

    // Game loop
    while (true) {
        long long curTime = clock();
        timer += curTime - preTime;
        preTime = curTime;
        if (timer >= tick) {
            // Fixed update
            timer -= tick;
            frameNum++;
            ScheduleAndFinishTestTasks(frameNum);
        }
    }
}

int main() {
    GLFWwindow* window = InitContext();
    if (!window) return 1;

    // State
    ImVec4 clear_color = ImColor(204, 234, 244);

    // 开启游戏主循环
    thread gameThread(GameLoopProc);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 渲染依赖图
        TaskAnt::AntWatcher::GetInstance()->ImGuiRenderTick();

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

    Cleanup(window);

    return 0;
}