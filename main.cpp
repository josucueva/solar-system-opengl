#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
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

struct PlanetData {
    std::string name;
    float orbitSpeed;
    float orbitRadius;
    float size;
    std::string texture;
};

std::vector<PlanetData> loadPlanetsFromCSV(const std::string& filepath);
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

    std::vector<PlanetData> planetsData = loadPlanetsFromCSV("assets/data/planets.csv");
    std::vector<CelestialBody*> planets;
    CelestialBody* earthPtr = nullptr;
    
    for (const auto& planetData : planetsData) {
        CelestialBody* planet = new CelestialBody(planetData.size, planetData.texture.c_str());
        planet->setOrbit(planetData.orbitRadius, planetData.orbitSpeed);
        planets.push_back(planet);
        
        if (planetData.name == "Earth") {
            earthPtr = planet;
        }
    }
    
    CelestialBody moon(0.0243f, "assets/textures/2k_moon.jpg");
    moon.setOrbit(0.005f, 1.0f);
    if (earthPtr) {
        moon.setParent(earthPtr);
    }

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);

        // Render background with texture shader (no lighting)
        glDepthMask(GL_FALSE);
        background.render(textureShader, view, projection);
        glDepthMask(GL_TRUE);

        // Render sun with texture shader (no lighting)
        sun.update(deltaTime);
        sun.render(textureShader, view, projection);

        // Setup light shader for planets with Phong lighting
        lightShader.use();
        lightShader.setVec3("sunPos", sun.getPosition());

        // Render planets with light shader
        for (auto* planet : planets) {
            planet->update(deltaTime);
            planet->render(lightShader, view, projection);
        }

        // Render moon with light shader
        moon.update(deltaTime);
        moon.render(lightShader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto* planet : planets) {
        delete planet;
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

void framebufferSizeCallback(GLFWwindow* /* window */, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* /* window */, double xposIn, double yposIn) {
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

void scrollCallback(GLFWwindow* /* window */, double /* xoffset */, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

std::vector<PlanetData> loadPlanetsFromCSV(const std::string& filepath) {
    std::vector<PlanetData> planets;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << std::endl;
        return planets;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        PlanetData planet;
        std::string token;
        
        std::getline(ss, planet.name, ',');
        std::getline(ss, token, ',');
        planet.orbitSpeed = std::stof(token);
        std::getline(ss, token, ',');
        planet.orbitRadius = std::stof(token);
        std::getline(ss, token, ',');
        planet.size = std::stof(token);
        std::getline(ss, planet.texture, ',');
        
        planets.push_back(planet);
    }
    
    file.close();
    return planets;
}
