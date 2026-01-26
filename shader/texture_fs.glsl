#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LocalPos;

uniform sampler2D texture1;
uniform vec3 sunPos;
uniform bool isSun;

void main()
{
    // Calculate texture coordinates using LocalPos
    vec3 normalizedPos = normalize(LocalPos);
    float u = 0.5 + atan(normalizedPos.z, normalizedPos.x) / (2.0 * 3.14159265359);
    float v = 0.5 - asin(normalizedPos.y) / 3.14159265359;
    vec2 texCoords = vec2(u, v);
    
    // Get texture color
    vec3 texColor = texture(texture1, texCoords).rgb;
    
    // If it's the sun, no lighting
    if (isSun) {
        FragColor = vec4(texColor, 1.0);
        return;
    }
    
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
