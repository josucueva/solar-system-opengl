#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "includes/camera.h"
#include "includes/shader.h"
#include "includes/texture.h"
#include "includes/body.h"

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700

using namespace std;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLFWwindow* initWindow(int width, int height, const char* title);
bool initGLEW();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main() {

    GLFWwindow* window = initWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Solar System");
    if (!window) {
        return -1;
    }

    if (!initGLEW()) {
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader lightShader("shader/light_vs.glsl", "shader/light_fs.glsl");
    Shader colorShader("shader/colors_vs.glsl", "shader/colors_fs.glsl");
    Shader textureShader("shader/texture_vs.glsl", "shader/texture_fs.glsl");

    CelestialBody sun(1.0f, "assets/textures/2k_sun.jpg");
    CelestialBody background(100.0f, "assets/textures/2k_stars.jpg");

    CelestialBody mercury(0.0349f, "assets/textures/2k_mercury.jpg");
    mercury.setOrbit(0.0129f, 1.0f);

    CelestialBody venus(0.0866f, "assets/textures/2k_venus_surface.jpg");
    venus.setOrbit(0.0241f, 0.7316f);
    
    CelestialBody earth(0.0911f, "assets/textures/2k_uranus.jpg");
    earth.setOrbit(0.0333f, 0.6221f);
    
    CelestialBody moon(0.0243f, "assets/textures/2k_moon.jpg");
    moon.setOrbit(0.005f, 1.0f);
    moon.setParent(&earth);

    CelestialBody mars(0.0485f, "assets/textures/2k_mars.jpg");
    mars.setOrbit(0.0507f, 0.5028f);

    CelestialBody jupiter(1.0f, "assets/textures/2k_jupiter.jpg");
    jupiter.setOrbit(0.1732f, 0.273f);

    CelestialBody saturn(0.8329f, "assets/textures/2k_saturn.jpg");
    saturn.setOrbit(0.319f, 0.2024f);

    CelestialBody uranus(0.3628f, "assets/textures/2k_uranus.jpg");
    uranus.setOrbit(0.6387f, 0.1423f);

    CelestialBody neptune(0.3522f, "assets/textures/2k_neptune.jpg");
    neptune.setOrbit(0.1f, 0.1134f);

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);

        glDepthMask(GL_FALSE);
        background.render(textureShader, view, projection);
        glDepthMask(GL_TRUE);

        sun.update(deltaTime);
        
        lightShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, sun.getPosition());
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        
        sun.render(textureShader, view, projection);

        mercury.update(deltaTime);
        mercury.render(textureShader, view, projection);

        venus.update(deltaTime);
        venus.render(textureShader, view, projection);
        
        earth.update(deltaTime);
        earth.render(textureShader, view, projection);

        moon.update(deltaTime);
        moon.render(textureShader, view, projection);
        
        mars.update(deltaTime);
        mars.render(textureShader, view, projection);

        jupiter.update(deltaTime);
        jupiter.render(textureShader, view, projection);

        saturn.update(deltaTime);
        saturn.render(textureShader, view, projection);

        uranus.update(deltaTime);
        uranus.render(textureShader, view, projection);

        neptune.update(deltaTime);
        neptune.render(textureShader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
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

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
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
