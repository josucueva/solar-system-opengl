#include "ring.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265358979323846

Ring::Ring(float innerRad, float outerRad, const char *texturePath)
    : innerRadius(innerRad), outerRadius(outerRad), position(0.0f),
      rotationAngle(0.0f), tiltAngle(0.0f) {

  texture = new Texture(texturePath);

  float *vertices;
  unsigned int *indices;
  int vertexCount, idxCount;

  createRingGeometry(vertices, vertexCount, indices, idxCount);
  setupMesh(vertices, vertexCount, indices, idxCount);

  delete[] vertices;
  delete[] indices;
}

Ring::~Ring() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
  delete texture;
}

void Ring::setTilt(float angle) { tiltAngle = angle; }

void Ring::setRotation(float angle) { rotationAngle = angle; }

void Ring::setPosition(const vec3 &pos) { position = pos; }

void Ring::update(const vec3 &parentPos, float parentRotation) {
  position = parentPos;
  rotationAngle = parentRotation;
}

void Ring::render(Shader &shader, const mat4 &view, const mat4 &projection) {
  shader.use();

  mat4 model = mat4(1.0f);
  model = translate(model, position);
  model = rotate(model, radians(rotationAngle), vec3(0.0f, 1.0f, 0.0f));
  model = rotate(model, radians(tiltAngle), vec3(1.0f, 0.0f, 0.0f));

  shader.setMat4("model", model);
  shader.setMat4("view", view);
  shader.setMat4("projection", projection);

  texture->bind(0);
  shader.setInt("texture1", 0);

  // Enable blending for transparent rings
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glDisable(GL_BLEND);
}

void Ring::createRingGeometry(float *&vertices, int &vertexCount,
                              unsigned int *&indices, int &idxCount) {
  // Each vertex has: position (3) + normal (3) + texCoords (2) = 8 floats
  // We create 2 rings of vertices (inner and outer)
  vertexCount = (SEGMENTS + 1) * 2;
  vertices = new float[vertexCount * 8];

  float angleStep = 2.0f * PI / SEGMENTS;
  int vIndex = 0;

  for (int i = 0; i <= SEGMENTS; ++i) {
    float angle = i * angleStep;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    // Inner vertex
    vertices[vIndex++] = innerRadius * cosA;  // x
    vertices[vIndex++] = 0.0f;                // y
    vertices[vIndex++] = innerRadius * sinA;  // z
    vertices[vIndex++] = 0.0f;                // nx
    vertices[vIndex++] = 1.0f;                // ny
    vertices[vIndex++] = 0.0f;                // nz
    vertices[vIndex++] = 0.0f;                // u (inner edge)
    vertices[vIndex++] = (float)i / SEGMENTS; // v

    // Outer vertex
    vertices[vIndex++] = outerRadius * cosA;  // x
    vertices[vIndex++] = 0.0f;                // y
    vertices[vIndex++] = outerRadius * sinA;  // z
    vertices[vIndex++] = 0.0f;                // nx
    vertices[vIndex++] = 1.0f;                // ny
    vertices[vIndex++] = 0.0f;                // nz
    vertices[vIndex++] = 1.0f;                // u (outer edge)
    vertices[vIndex++] = (float)i / SEGMENTS; // v
  }

  // Create indices for triangles
  idxCount = SEGMENTS * 6; // 2 triangles per segment
  indices = new unsigned int[idxCount];
  int idx = 0;

  for (int i = 0; i < SEGMENTS; ++i) {
    int current = i * 2;
    int next = (i + 1) * 2;

    // First triangle (counter-clockwise from top)
    indices[idx++] = current;
    indices[idx++] = current + 1;
    indices[idx++] = next;

    // Second triangle
    indices[idx++] = current + 1;
    indices[idx++] = next + 1;
    indices[idx++] = next;
  }

  indexCount = idxCount;
}

void Ring::setupMesh(const float *vertices, int vertexCount,
                     const unsigned int *indices, int idxCount) {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexCount * 8 * sizeof(float), vertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxCount * sizeof(unsigned int),
               indices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Texture coordinate attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}
