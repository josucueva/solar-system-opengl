#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LocalPos;

uniform sampler2D texture1;

// sun position
uniform vec3 sunPos;
uniform vec3 viewPos;

// material reflection coefficients
uniform vec3 material_Ka;
uniform vec3 material_Kd;
uniform vec3 material_Ks;
uniform float material_shininess;

// light intensity components
uniform vec3 light_La;
uniform vec3 light_Ld;
uniform vec3 light_Le;

void main()
{
    // spherical texture mapping
    vec3 normalizedPos = normalize(LocalPos);
    float u = 0.5 + atan(normalizedPos.z, normalizedPos.x) / (2.0 * 3.14159265359);
    float v = 0.5 - asin(normalizedPos.y) / 3.14159265359;
    vec2 texCoords = vec2(u, v);
    vec3 texColor = texture(texture1, texCoords).rgb;
    
    // phong lighting model
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(sunPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    // distance attenuation (adjusted for astronomical distances)
    float distance = length(sunPos - FragPos);
    // weaker attenuation for space: constant + linear term
    float attenuation = 1.0 + 0.00002 * distance;
    
    // ambient component
    vec3 I_ambient = material_Ka * light_La;
    
    // diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 I_diffuse = (material_Kd * light_Ld * diff) / attenuation;
    
    // specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    vec3 I_specular = (material_Ks * light_Le * spec) / attenuation;
    
    // combine lighting with texture
    vec3 lighting = I_ambient + I_diffuse + I_specular;
    vec3 finalColor = lighting * texColor;
    
    FragColor = vec4(finalColor, 1.0);
}
