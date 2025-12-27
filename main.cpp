#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

#include "includes/camera.h"

#define PI 3.14159265358979323846
#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700
#define STACKS 30
#define SECTORS 30

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


using namespace std;

typedef unsigned int uint;

typedef struct vertex {
    float x, y, z;      // position
    float nx, ny, nz;   // normal
} vertex;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

string loadShaderFromFile(const char* filePath);
void processInput(GLFWwindow *window);
void createVao(uint &VAO, uint &VBO, vertex* vertices, size_t size);
GLFWwindow* initWindow(int width, int height, const char* title);
bool initGLEW();
uint compileShader(uint type, const char* source);
uint createShaderProgram(uint vertexShader, uint fragmentShader);
const vertex* createSphereVertices(vertex center, float radius, const int stacks, const int sectors);
uint* createSphereIndices(const int stacks, const int sectors, int& indexCount);
void initLighting();
uint loadTexture(const char* path);


int main() {

    GLFWwindow* window = initWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sphere");

    if (!window) {
        return -1;
    }

    if (!initGLEW()) {
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    uint colorsVS = compileShader(GL_VERTEX_SHADER, loadShaderFromFile("shader/colors_vs.glsl").c_str());
    uint colorsFS = compileShader(GL_FRAGMENT_SHADER, loadShaderFromFile("shader/colors_fs.glsl").c_str());
    uint colorsShaderProgram = createShaderProgram(colorsVS, colorsFS);

    uint lightVS = compileShader(GL_VERTEX_SHADER, loadShaderFromFile("shader/light_vs.glsl").c_str());
    uint lightFS = compileShader(GL_FRAGMENT_SHADER, loadShaderFromFile("shader/light_fs.glsl").c_str());
    uint lightShaderProgram = createShaderProgram(lightVS, lightFS);

    uint textureVS = compileShader(GL_VERTEX_SHADER, loadShaderFromFile("shader/texture_vs.glsl").c_str());
    uint textureFS = compileShader(GL_FRAGMENT_SHADER, loadShaderFromFile("shader/texture_fs.glsl").c_str());
    uint textureShaderProgram = createShaderProgram(textureVS, textureFS);

    glDeleteShader(colorsVS);
    glDeleteShader(colorsFS);
    glDeleteShader(lightVS);
    glDeleteShader(lightFS);
    glDeleteShader(textureVS);
    glDeleteShader(textureFS);

    const vertex* vertices = createSphereVertices({0.0f, 0.0f, 0.0f}, 0.5f, STACKS, SECTORS);
    const int vertexCount = (STACKS + 1) * (SECTORS + 1);
    
    // generate indices for the sphere
    int indexCount = 0;
    uint* indices = createSphereIndices(STACKS, SECTORS, indexCount);
    
    uint sphereVAO, VBO, IBO;
    
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vertex), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location = 1) - offset by 3 floats for position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create VAO for the light source (reuses same VBO and IBO as sphere)
    uint lightVAO;

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // Bind the same VBO (contains all sphere vertex data)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Bind the same IBO (index buffer binding is per-VAO, so we must bind it here too)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Position attribute only for light cube (we don't need normals for the light source)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Create background sphere VAO (large sphere with stars texture)
    const vertex* bgVertices = createSphereVertices({0.0f, 0.0f, 0.0f}, 50.0f, STACKS, SECTORS);
    int bgIndexCount = 0;
    uint* bgIndices = createSphereIndices(STACKS, SECTORS, bgIndexCount);
    
    uint bgVAO, bgVBO, bgIBO;
    glGenVertexArrays(1, &bgVAO);
    glGenBuffers(1, &bgVBO);
    glGenBuffers(1, &bgIBO);

    glBindVertexArray(bgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vertex), bgVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bgIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bgIndexCount * sizeof(uint), bgIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    uint spaceTexture = loadTexture("assets/textures/2k_stars.jpg");

    while(!glfwWindowShouldClose(window)) {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

        glDepthMask(GL_FALSE);
        glUseProgram(textureShaderProgram);
        glm::mat4 bgModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(textureShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(bgModel));
        glUniformMatrix4fv(glGetUniformLocation(textureShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(textureShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindTexture(GL_TEXTURE_2D, spaceTexture);
        glBindVertexArray(bgVAO);
        glDrawElements(GL_TRIANGLES, bgIndexCount, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE); // Re-enable depth writing

        // draw light source
        glUseProgram(lightShaderProgram);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    
        // draw sphere with lighting
        glUseProgram(colorsShaderProgram);
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(colorsShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(colorsShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(colorsShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        // Set lighting uniforms
        glUniform3f(glGetUniformLocation(colorsShaderProgram, "objectColor"), 0.66f, 0.67f, 0.67f);
        glUniform3f(glGetUniformLocation(colorsShaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(colorsShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(colorsShaderProgram, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &bgVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteBuffers(1, &bgVBO);
    glDeleteBuffers(1, &bgIBO);
    glDeleteTextures(1, &spaceTexture);
    
    delete[] vertices;
    delete[] indices;
    delete[] bgVertices;
    delete[] bgIndices;

    glfwTerminate();
    return 0;
}

string loadShaderFromFile(const char* filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open shader file: " << filePath << endl;
        return "";
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}

GLFWwindow* initWindow(int width, int height, const char* title) {
    
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    // Capture mouse cursor for camera control
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glViewport(0, 0, width, height);

    return window;
}

bool initGLEW() {

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    glGetError();

    if (glewError != GLEW_OK && glewError != GLEW_ERROR_NO_GLX_DISPLAY) {
        
        cerr << "GLEW initialization reported: " << glewGetErrorString(glewError) << endl;
        
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        
        if (glGetError() != GL_NO_ERROR) {
            cerr << "OpenGL context is not working properly" << endl;
            return false;
        }
        
        cout << "Warning: GLEW reported an error, but OpenGL context seems to work. Continuing..." << endl;
        cout << "Detected OpenGL " << major << "." << minor << endl;
    }

    return true;
}

uint compileShader(uint type, const char* source) {
    
    uint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        cerr << "Shader compilation failed:\n" << infoLog << endl;
    }

    return shader;
}

uint createShaderProgram(uint vertexShader, uint fragmentShader) {
    
    uint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        cerr << "Shader program linking failed:\n" << infoLog << endl;
    }

    return program;
}

void createVao(uint &VAO, uint &VBO, vertex* vertices, size_t size) {
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

const vertex* createSphereVertices(vertex center, float radius, const int stackCount, const int sectorCount) {

    vertex* vertices = new vertex[(stackCount + 1) * (sectorCount + 1)];

    float sectorStep = 2.0f * PI / sectorCount;
    float stackStep = PI / stackCount;
    
    int index = 0;
    
    for (int i = 0; i <= stackCount; ++i) {
        
        float stackAngle = PI / 2.0f - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            
            float sectorAngle = j * sectorStep;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            float px = center.x + x;
            float py = center.y + y;
            float pz = center.z + z;
            
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            
            vertices[index++] = {px, py, pz, nx, ny, nz};
        }
    }
    
    return vertices;
}

uint* createSphereIndices(const int stackCount, const int sectorCount, int& indexCount) {
    
    indexCount = stackCount * sectorCount * 6;
    uint* indices = new uint[indexCount];
    
    int index = 0;
    
    for (int i = 0; i < stackCount; ++i) {
        
        int k1 = i * (sectorCount + 1);        
        int k2 = k1 + sectorCount + 1;          
        
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            
            // 2 triangles per sector excluding first and last stacks
            // Triangle 1: k1 => k2 => k1+1
            if (i != 0) {
                indices[index++] = k1;
                indices[index++] = k2;
                indices[index++] = k1 + 1;
            }
            
            // Triangle 2: k1+1 => k2 => k2+1
            if (i != (stackCount - 1)) {
                indices[index++] = k1 + 1;
                indices[index++] = k2;
                indices[index++] = k2 + 1;
            }
        }
    }
    
    indexCount = index;
    
    return indices;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

uint loadTexture(const char* path) {
    uint textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
        cout << "Texture loaded: " << path << " (" << width << "x" << height << ")" << endl;
    } else {
        cerr << "Failed to load texture: " << path << endl;
        stbi_image_free(data);
    }
    
    return textureID;
}