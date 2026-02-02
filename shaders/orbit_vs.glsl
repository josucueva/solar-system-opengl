#version 330 core
layout (location = 0) in vec3 aPos;  // circle points in object space

uniform mat4 model;      // positions the orbit circle in world space
uniform mat4 view;       // view from camera
uniform mat4 projection; // perspective

void main()
{
    // transform orbit line from object space all the way to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}