#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct PointLight {
    vec3 position;
    vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform PointLight pointLight;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * pointLight.color;

    // Diffuse
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuseStrength * diff * pointLight.color;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = pointLight.specularStrength * spec * pointLight.color;

    // Calculate attenuation
    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    vec3 lighting = (ambient + diffuse + specular) * attenuation;

    FragColor = vec4(lighting, 1.0);
}
