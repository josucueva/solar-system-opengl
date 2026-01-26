#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LocalPos;

uniform sampler2D texture1;
uniform vec3 sunPos;

void main()
{
    // Calculate texture coordinates using Normal (rotates with object)
    vec3 normalizedNormal = normalize(Normal);
    float u = 0.5 + atan(normalizedNormal.z, normalizedNormal.x) / (2.0 * 3.14159265359);
    float v = 0.5 - asin(normalizedNormal.y) / 3.14159265359;
    vec2 texCoords = vec2(u, v);
    
    // Get texture color
    vec3 texColor = texture(texture1, texCoords).rgb;
    
    // Phong lighting
    vec3 n = normalize(Normal);
    vec3 l = normalize(sunPos - FragPos);
    
    // Ambient (very low)
    float ka = 0.05;
    
    // Diffuse
    float kd = 0.95;
    float diff = max(dot(n, l), 0.0);
    
    // Combine: ambient + diffuse
    float lightIntensity = ka + kd * diff;
    vec3 finalColor = lightIntensity * texColor;
    
    FragColor = vec4(finalColor, 1.0);
}
