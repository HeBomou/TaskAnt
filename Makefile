EXE = task_ant
SOURCES = main.cpp
SOURCES += AntWatcher.cpp
SOURCES += Ant.cpp AntEvent.cpp AntManager.cpp AntTask.cpp AntThread.cpp AntThreadProc.cpp UniversalPlatformAntThread.cpp
SOURCES += imgui_impl_glfw.cpp imgui_impl_opengl3.cpp
SOURCES += imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_widgets.cpp
SOURCES += ImNodes.cpp ImNodesEz.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
CXXFLAGS = -Ilibs/imgui/
CXXFLAGS += -Ilibs/ImNodes/
CXXFLAGS += -I/usr/local/include
CXXFLAGS += -g -Wall -Wformat
CXXFLAGS += -std=c++17
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
LIBS += -L/usr/local/lib
LIBS += -lGLEW
LIBS += -lglfw
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o:TaskAnt/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o:libs/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o:libs/ImNodes/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
all: $(EXE)
	@echo Build complete
$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
clean:
	rm -f $(EXE) $(OBJS)