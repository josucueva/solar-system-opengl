#include "orbit.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265358979323846

Orbit::Orbit(float orbitRadius, const glm::vec3 &orbitColor)
    : vertexCount(SEGMENTS), radius(orbitRadius), color(orbitColor) {
  setupOrbit();
}

Orbit::~Orbit() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

void Orbit::setupOrbit() {
  // create circle vertices
  float *vertices = new float[SEGMENTS * 3];

  for (int i = 0; i < SEGMENTS; i++) {
    float angle = (2.0f * PI * i) / SEGMENTS;
    vertices[i * 3 + 0] = radius * cos(angle);
    vertices[i * 3 + 1] = 0.0f;
    vertices[i * 3 + 2] = radius * sin(angle);
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, SEGMENTS * 3 * sizeof(float), vertices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  delete[] vertices;
}

void Orbit::render(Shader &shader, const glm::mat4 &view,
                   const glm::mat4 &projection) {
  shader.use();

  glm::mat4 model = glm::mat4(1.0f);

  shader.setMat4("model", model);
  shader.setMat4("view", view);
  shader.setMat4("projection", projection);
  shader.setVec3("color", color);

  glBindVertexArray(VAO);
  glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
  glBindVertexArray(0);
}
