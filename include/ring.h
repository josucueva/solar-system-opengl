#ifndef RING_H
#define RING_H

#include "shader.h"
#include "texture.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

class Ring {
private:
  float innerRadius;
  float outerRadius;
  vec3 position;
  float rotationAngle;
  float tiltAngle;

  unsigned int VAO, VBO, IBO;
  int indexCount;
  Texture *texture;

  static const int SEGMENTS = 100;

  void setupMesh(const float *vertices, int vertexCount,
                 const unsigned int *indices, int idxCount);
  void createRingGeometry(float *&vertices, int &vertexCount,
                          unsigned int *&indices, int &idxCount);

public:
  Ring(float innerRad, float outerRad, const char *texturePath);
  ~Ring();

  void setTilt(float angle);
  void setRotation(float angle);
  void setPosition(const vec3 &pos);
  void update(float deltaTime, const vec3 &parentPos, float parentRotation);
  void render(Shader &shader, const mat4 &view, const mat4 &projection);
};

#endif
