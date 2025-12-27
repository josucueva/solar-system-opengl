#version 330 core

layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec3 aNormal;   // Vertex normal

out vec3 FragPos;    // Fragment position in world space
out vec3 Normal;     // Normal vector in world space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Transform vertex position to world space for lighting calculations
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normal to world space (using normal matrix to handle non-uniform scaling)
    // For uniform scaling, this simplifies to mat3(model), but proper way is transpose(inverse(model))
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Final position in clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
} 