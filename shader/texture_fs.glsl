#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;

void main()
{
    vec3 normalizedPos = normalize(FragPos);
    
    float u = 0.5 + atan(normalizedPos.z, normalizedPos.x) / (2.0 * 3.14159265359);
    float v = 0.5 - asin(normalizedPos.y) / 3.14159265359;
    
    vec2 texCoords = vec2(u, v);
    FragColor = texture(texture1, texCoords);
}
