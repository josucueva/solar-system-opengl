ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Iinclude -Iexternal
SRCS := src/main.cpp src/shader.cpp src/texture.cpp src/body.cpp src/orbit.cpp src/ring.cpp
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))

ifeq ($(DETECTED_OS),Windows)
    TARGET := bin/solar_system.exe
    RM := del /Q
    LIBS := -lglew32 -lopengl32 -lglfw3 -lgdi32
else ifeq ($(DETECTED_OS),Linux)
    TARGET := bin/solar_system
    RM := rm -f
    LIBS := -lGLEW -lGL -lglfw
else ifeq ($(DETECTED_OS),Darwin)
    TARGET := bin/solar_system
    RM := rm -f
    LIBS := -lGLEW -lglfw -framework OpenGL
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(DETECTED_OS),Windows)
	$(RM) build\\*.o bin\\$(TARGET)
else
	$(RM) build/*.o $(TARGET)
endif

run: $(TARGET)
	./$(TARGET)

rebuild: clean all

.PHONY: all clean run rebuild
