#ifndef BODY_H
#define BODY_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "shader.h"
#include "texture.h"

struct Vertex {
    float x, y, z;      // position
    float nx, ny, nz;   // normal
};

class CelestialBody {
protected:
    glm::vec3 position;
    float radius;
    float rotationAngle;
    float rotationSpeed;
    
    float orbitRadius;
    float orbitSpeed;
    float orbitAngle;
    
    CelestialBody* parent;
    
    unsigned int VAO, VBO, IBO;
    int indexCount;
    Texture* texture;
    
    static const int STACKS = 30;
    static const int SECTORS = 30;
    
    void setupMesh(const Vertex* vertices, int vertexCount, const unsigned int* indices, int idxCount);
    
public:
    CelestialBody(float rad, const char* texturePath);
    virtual ~CelestialBody();
    void setOrbit(float orbRadius, float orbSpeed);
    void setParent(CelestialBody* parentBody);
    virtual void update(float deltaTime);
    virtual void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    
    glm::vec3 getPosition() const { return position; }
    float getRadius() const { return radius; }
    
    static Vertex* createSphereVertices(glm::vec3 center, float radius, int stacks, int sectors, int& vertexCount);
    static unsigned int* createSphereIndices(int stacks, int sectors, int& indexCount);
};

#endif
