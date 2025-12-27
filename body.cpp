#include "includes/body.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#define PI 3.14159265358979323846

CelestialBody::CelestialBody(float rad, const char* texturePath) 
    : radius(rad), position(0.0f), rotationAngle(0.0f), rotationSpeed(0.5f),
      orbitRadius(0.0f), orbitSpeed(0.0f), orbitAngle(0.0f), parent(nullptr) {
    
    texture = new Texture(texturePath);
    
    int vertexCount;
    Vertex* vertices = createSphereVertices(glm::vec3(0.0f), radius, STACKS, SECTORS, vertexCount);
    unsigned int* indices = createSphereIndices(STACKS, SECTORS, indexCount);
    
    setupMesh(vertices, vertexCount, indices, indexCount);
    
    delete[] vertices;
    delete[] indices;
}

CelestialBody::~CelestialBody() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    delete texture;
}

void CelestialBody::setOrbit(float orbRadius, float orbSpeed) {
    orbitRadius = orbRadius * 100;
    orbitSpeed = orbSpeed * 50;
}

void CelestialBody::setParent(CelestialBody* parentBody) {
    parent = parentBody;
}

void CelestialBody::update(float deltaTime) {

    rotationAngle += rotationSpeed * deltaTime;
    if (rotationAngle > 360.0f) rotationAngle -= 360.0f;
    
    if (orbitRadius > 0.0f) {
        orbitAngle += orbitSpeed * deltaTime;
        if (orbitAngle > 360.0f) orbitAngle -= 360.0f;
        
        glm::vec3 orbitOffset;
        orbitOffset.x = orbitRadius * cos(glm::radians(orbitAngle));
        orbitOffset.y = 0.0f;
        orbitOffset.z = orbitRadius * sin(glm::radians(orbitAngle));
        
        if (parent) {
            position = parent->getPosition() + orbitOffset;
        } else {
            position = orbitOffset;
        }
    }
}

void CelestialBody::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    
    shader.use();
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    texture->bind(0);
    shader.setInt("texture1", 0);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CelestialBody::setupMesh(const Vertex* vertices, int vertexCount, const unsigned int* indices, int idxCount) {
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

Vertex* CelestialBody::createSphereVertices(glm::vec3 center, float radius, int stackCount, int sectorCount, int& vertexCount) {
    
    vertexCount = (stackCount + 1) * (sectorCount + 1);
    Vertex* vertices = new Vertex[vertexCount];
    
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

unsigned int* CelestialBody::createSphereIndices(int stackCount, int sectorCount, int& indexCount) {
    indexCount = stackCount * sectorCount * 6;
    unsigned int* indices = new unsigned int[indexCount];
    
    int index = 0;
    
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices[index++] = k1;
                indices[index++] = k2;
                indices[index++] = k1 + 1;
            }
            
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
