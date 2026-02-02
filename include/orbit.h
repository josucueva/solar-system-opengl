#ifndef ORBIT_H
#define ORBIT_H

#include "shader.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class Orbit {
private:
  unsigned int VAO, VBO;
  int vertexCount;
  float radius;
  glm::vec3 color;

  static const int SEGMENTS = 360;

  void setupOrbit();

public:
  Orbit(float orbitRadius, const glm::vec3 &orbitColor = glm::vec3(1.0f));
  ~Orbit();

  void render(Shader &shader, const glm::mat4 &view,
              const glm::mat4 &projection,
              const glm::mat4 &model = glm::mat4(1.0f));
};

#endif
