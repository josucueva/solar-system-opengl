#version 330 core
layout (location = 0) in vec3 aPos;  // vertex position in object space
layout (location = 1) in vec3 aNormal;  // surface normal for lighting

out vec3 FragPos;  // position in world space
out vec3 Normal;   // normal in world space
out vec3 LocalPos; // original position in object space (for texture mapping)

uniform mat4 model;      // object space -> world space
uniform mat4 view;       // world space -> camera space
uniform mat4 projection; // camera space -> clip space

void main()
{
    // transform position to world space for lighting
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // transform normal to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // keep original position for texture wrapping around the sphere
    LocalPos = aPos;
    
    // apply all transformations from object space to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
