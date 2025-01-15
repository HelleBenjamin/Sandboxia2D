CXX = g++
CXXFLAGS = -lGL -lglfw -Iinclude
BUILD_DIR = build

IMGUI_SRC = src/imgui/imgui.cpp src/imgui/imgui_demo.cpp src/imgui/imgui_draw.cpp src/imgui/imgui_tables.cpp src/imgui/imgui_widgets.cpp src/imgui/imgui_impl_glfw.cpp src/imgui/imgui_impl_opengl3.cpp
IMGUI_OBJ = $(IMGUI_SRC:src/%.cpp=$(BUILD_DIR)/%.o)

SRC = src/main.cpp src/renderer.cpp src/player.cpp src/ui.cpp src/glad.c
OBJ = $(SRC:src/%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BUILD_DIR)/Sandboxia

all: $(TARGET)

$(TARGET): $(IMGUI_OBJ) $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(BUILD_DIR)
	rm imgui.ini

