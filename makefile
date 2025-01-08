CXX = g++
CXXFLAGS = -lGL -lglfw -Iinclude


all:
	$(CXX) -o Sandboxia src/main.cpp src/renderer.cpp src/player.cpp src/glad.c $(CXXFLAGS)
