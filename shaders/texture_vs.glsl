#version 330 core
layout (location = 0) in vec3 aPos;  // vertex in object space
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;  // world space position
out vec3 Normal;   // world space normal

uniform mat4 model;      // object -> world
uniform mat4 view;       // world -> camera
uniform mat4 projection; // camera -> clip

void main()
{
    // transform position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // final transformation through all spaces
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
