#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "body.h"
#include "camera.h"
#include "orbit.h"
#include "shader.h"
#include "texture.h"

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700

using namespace std;
using namespace glm;

// sun properties
const float SUN_SIZE = 1.0f;
const char *SUN_TEXTURE = "assets/textures/2k_sun.jpg";

// scale configuration
const float PLANET_SIZE_SCALE = 1.0f;
const float DISTANCE_SCALE = 1.0f;
const float SPEED_SCALE = 1.0f;

// background properties
const float BACKGROUND_SIZE = 500.0f;
const char *BACKGROUND_TEXTURE = "assets/textures/2k_stars_milky_way.jpg";

// moon properties (relative to Earth)
const float MOON_SIZE = 0.273f * PLANET_SIZE_SCALE;
const float MOON_ORBIT_RADIUS = 0.087f;
const float MOON_ORBIT_SPEED = 13.4f;
const char *MOON_TEXTURE = "assets/textures/2k_moon.jpg";

// orbit rendering
const vec3 ORBIT_COLOR = vec3(1.0f, 1.0f, 1.0f);
const bool DRAW_ORBITS = true;

// sun light intensities
const vec3 LIGHT_AMBIENT = vec3(0.15f, 0.15f, 0.15f);
const vec3 LIGHT_DIFFUSE = vec3(1.0f, 1.0f, 1.0f);
const vec3 LIGHT_SPECULAR = vec3(1.0f, 1.0f, 1.0f);

// rocky planet material
const vec3 ROCKY_KA = vec3(0.25f, 0.25f, 0.25f);
const vec3 ROCKY_KD = vec3(0.8f, 0.8f, 0.8f);
const vec3 ROCKY_KS = vec3(0.3f, 0.3f, 0.3f);
const float ROCKY_SHININESS = 32.0f;

// gas giant material
const vec3 GAS_KA = vec3(0.3f, 0.3f, 0.3f);
const vec3 GAS_KD = vec3(0.9f, 0.9f, 0.9f);
const vec3 GAS_KS = vec3(0.5f, 0.5f, 0.5f);
const float GAS_SHININESS = 64.0f;

Camera camera(vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

vec3 lightPos(1.2f, 1.0f, 2.0f);

struct PlanetData {
  string name;
  float orbitSpeed;
  float orbitRadius;
  float size;
  string texture;
  float rotationSpeed;
  string type;
};

vector<PlanetData> loadPlanetsFromCSV(const string &filepath);
GLFWwindow *initWindow(int width, int height, const char *title);
bool initGLEW();
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main() {

  GLFWwindow *window = initWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Solar System");
  if (!window) {
    return -1;
  }

  if (!initGLEW()) {
    glfwTerminate();
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  Shader lightShader("shaders/light_vs.glsl", "shaders/light_fs.glsl");
  Shader colorShader("shaders/colors_vs.glsl", "shaders/colors_fs.glsl");
  Shader textureShader("shaders/texture_vs.glsl", "shaders/texture_fs.glsl");
  Shader orbitShader("shaders/orbit_vs.glsl", "shaders/orbit_fs.glsl");

  // scale sun size for visibility (sun is actually 109x Earth diameter)
  CelestialBody sun(SUN_SIZE * PLANET_SIZE_SCALE, SUN_TEXTURE);
  sun.setRotationSpeed(10.0f);

  CelestialBody background(BACKGROUND_SIZE, BACKGROUND_TEXTURE);

  vector<PlanetData> planetsData =
      loadPlanetsFromCSV("assets/data/planets.csv");
  vector<CelestialBody *> planets;
  vector<Orbit *> orbits;
  CelestialBody *earthPtr = nullptr;

  for (const auto &planetData : planetsData) {
    // apply size scaling for visibility
    CelestialBody *planet = new CelestialBody(
        planetData.size * PLANET_SIZE_SCALE, planetData.texture.c_str());

    // apply distance and speed scaling
    planet->setOrbit(planetData.orbitRadius * DISTANCE_SCALE,
                     planetData.orbitSpeed * SPEED_SCALE);
    planet->setRotationSpeed(planetData.rotationSpeed);

    // assign material properties based on planet type
    if (planetData.type == "gas") {
      planet->setMaterial(GAS_KA, GAS_KD, GAS_KS, GAS_SHININESS);
    } else {
      planet->setMaterial(ROCKY_KA, ROCKY_KD, ROCKY_KS, ROCKY_SHININESS);
    }

    planets.push_back(planet);

    // create orbit path
    Orbit *orbit =
        new Orbit(planetData.orbitRadius * DISTANCE_SCALE * 100, ORBIT_COLOR);
    orbits.push_back(orbit);

    if (planetData.name == "Earth") {
      earthPtr = planet;
    }
  }

  CelestialBody moon(MOON_SIZE, MOON_TEXTURE);
  moon.setOrbit(MOON_ORBIT_RADIUS, MOON_ORBIT_SPEED);
  moon.setRotationSpeed(50.0f);
  moon.setMaterial(ROCKY_KA, ROCKY_KD, ROCKY_KS, ROCKY_SHININESS);

  Orbit *moonOrbit = new Orbit(MOON_ORBIT_RADIUS * 100, ORBIT_COLOR);

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

    mat4 view = camera.GetViewMatrix();
    mat4 projection =
        perspective(radians(camera.Zoom),
                    (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 2000.0f);

    // render background
    glDepthMask(GL_FALSE);
    background.render(textureShader, view, projection);
    glDepthMask(GL_TRUE);

    // render sun
    sun.update(deltaTime);
    sun.render(textureShader, view, projection);

    // render orbit paths
    if (DRAW_ORBITS) {
      for (auto *orbit : orbits) {
        orbit->render(orbitShader, view, projection);
      }

      if (earthPtr) {
        mat4 moonOrbitModel = mat4(1.0f);
        moonOrbitModel = translate(moonOrbitModel, earthPtr->getPosition());
        moonOrbit->render(orbitShader, view, projection, moonOrbitModel);
      }
    }

    // setup lighting from sun
    lightShader.use();
    lightShader.setVec3("sunPos", sun.getPosition());
    lightShader.setVec3("viewPos", camera.Position);

    // set light properties (from the sun)
    lightShader.setVec3("light_La", LIGHT_AMBIENT);
    lightShader.setVec3("light_Ld", LIGHT_DIFFUSE);
    lightShader.setVec3("light_Le", LIGHT_SPECULAR);

    // render planets
    for (auto *planet : planets) {
      planet->update(deltaTime);
      planet->render(lightShader, view, projection);
    }

    // render moon
    moon.update(deltaTime);
    moon.render(lightShader, view, projection);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (auto *planet : planets) {
    delete planet;
  }

  for (auto *orbit : orbits) {
    delete orbit;
  }

  delete moonOrbit;

  glfwTerminate();
  return 0;
}

GLFWwindow *initWindow(int width, int height, const char *title) {
  if (!glfwInit()) {
    cerr << "Failed to initialize GLFW" << endl;
    return nullptr;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

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
    cerr << "GLEW initialization reported: " << glewGetErrorString(glewError)
         << endl;

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    if (glGetError() != GL_NO_ERROR) {
      cerr << "OpenGL context is not working properly" << endl;
      return false;
    }

    cout << "Warning: GLEW reported an error, but OpenGL context seems to "
            "work. Continuing..."
         << endl;
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

void framebufferSizeCallback(GLFWwindow * /* window */, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow * /* window */, double xposIn, double yposIn) {
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

void scrollCallback(GLFWwindow * /* window */, double /* xoffset */,
                    double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

vector<PlanetData> loadPlanetsFromCSV(const string &filepath) {
  vector<PlanetData> planets;
  ifstream file(filepath);

  if (!file.is_open()) {
    cerr << "Failed to open " << filepath << endl;
    return planets;
  }

  string line;
  getline(file, line); // skip header

  while (getline(file, line)) {
    stringstream ss(line);
    PlanetData planet;
    string token;

    getline(ss, planet.name, ',');
    getline(ss, token, ',');
    planet.orbitSpeed = stof(token);
    getline(ss, token, ',');
    planet.orbitRadius = stof(token);
    getline(ss, token, ',');
    planet.size = stof(token);
    getline(ss, planet.texture, ',');
    getline(ss, token, ',');
    planet.rotationSpeed = stof(token);
    getline(ss, planet.type, ',');

    planets.push_back(planet);
  }

  file.close();
  return planets;
}
