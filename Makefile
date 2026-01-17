ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra
SRCS := main.cpp includes/shader.cpp includes/texture.cpp includes/body.cpp
OBJS := $(SRCS:.cpp=.o)

ifeq ($(DETECTED_OS),Windows)
    TARGET := solar_system.exe
    RM := del /Q
    LIBS := -lglew32 -lopengl32 -lglfw3 -lgdi32
else ifeq ($(DETECTED_OS),Linux)
    TARGET := solar_system
    RM := rm -f
    LIBS := -lGLEW -lGL -lglfw
else ifeq ($(DETECTED_OS),Darwin)
    TARGET := solar_system
    RM := rm -f
    LIBS := -lGLEW -lglfw -framework OpenGL
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(DETECTED_OS),Windows)
	$(RM) $(OBJS) $(TARGET)
else
	$(RM) $(OBJS) $(TARGET)
endif

run: $(TARGET)
	./$(TARGET)

rebuild: clean all

.PHONY: all clean run rebuild
