all:
	g++ -o main src/main.cpp src/renderer.cpp src/player.cpp src/glad.c -lGL -lGLU -lglfw -Iinclude
