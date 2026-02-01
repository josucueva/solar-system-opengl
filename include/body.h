#ifndef BODY_H
#define BODY_H

#include "shader.h"
#include "texture.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

using namespace std;
using namespace glm;

struct Vertex {
  float x, y, z;    // position
  float nx, ny, nz; // normal
};

class CelestialBody {
protected:
  vec3 position;
  float radius;
  float rotationAngle;
  float rotationSpeed;

  float orbitRadius;
  float orbitSpeed;
  float orbitAngle;

  CelestialBody *parent;

  // material properties
  vec3 materialKa;         // ambient reflection
  vec3 materialKd;         // diffuse reflection
  vec3 materialKs;         // specular reflection
  float materialShininess; // specular exponent

  unsigned int VAO, VBO, IBO;
  int indexCount;
  Texture *texture;

  static const int STACKS = 30;
  static const int SECTORS = 30;

  void setupMesh(const Vertex *vertices, int vertexCount,
                 const unsigned int *indices, int idxCount);

public:
  CelestialBody(float rad, const char *texturePath);
  virtual ~CelestialBody();
  void setOrbit(float orbRadius, float orbSpeed);
  void setRotationSpeed(float speed);
  void setParent(CelestialBody *parentBody);
  void setMaterial(const vec3 &ka, const vec3 &kd, const vec3 &ks,
                   float shininess);
  virtual void update(float deltaTime);
  virtual void render(Shader &shader, const mat4 &view, const mat4 &projection);

  vec3 getPosition() const { return position; }
  float getRadius() const { return radius; }

  static Vertex *createSphereVertices(vec3 center, float radius, int stacks,
                                      int sectors, int &vertexCount);
  static unsigned int *createSphereIndices(int stacks, int sectors,
                                           int &indexCount);
};

#endif
