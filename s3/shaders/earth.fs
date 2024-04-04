#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 ViewDir; // Make sure to pass the view direction from the vertex shader

uniform sampler2D earthTexture;
uniform sampler2D earthNormalMap;
uniform sampler2D earthCloudTexture;
uniform sampler2D earthSpecularMap;

uniform vec3 lightDirection;
uniform float heightScale; // Controls the amount of parallax

// Atmospheric scattering parameters
const float atmosphereThickness = 0.01; // Adjust to control the thickness of the atmosphere
const vec3 atmosphereColor = vec3(0.5, 0.7, 1.0); // Adjust to control the color of the atmosphere

void main()
{
    // Parallax mapping for clouds
    vec3 viewDir = normalize(ViewDir);
    float height = texture(earthCloudTexture, TexCoords).r;
    vec2 parallaxTexCoords = TexCoords + (viewDir.xy * height * heightScale);

    float cloudHeight = texture(earthCloudTexture, parallaxTexCoords).r;
    float cloudIntensity = 0.8;
    float cloudOffset = 0.02;

    vec4 cloudColor = vec4(1.0, 1.0, 1.0, cloudHeight * cloudIntensity);

    vec4 earthColor = texture(earthTexture, TexCoords);
    vec3 normal = normalize(texture(earthNormalMap, TexCoords).rgb * 2.0 - 1.0);

    // Specular lighting
    float specularIntensity = texture(earthSpecularMap, TexCoords).r;
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * specularIntensity * vec3(1.0, 1.0, 1.0);

    // Blend the earth and cloud colors
    vec4 finalColor = mix(earthColor, cloudColor, cloudColor.a);
    finalColor.rgb += specular;

    // Apply atmospheric scattering
    float atmosphereFactor = max(1.0 - length(ViewDir) / atmosphereThickness, 0.0);
    vec3 atmosphere = atmosphereColor * atmosphereFactor * max(dot(normal, -lightDirection), 0.0);

    // Combine the final color with the atmosphere
    finalColor.rgb = mix(finalColor.rgb, atmosphere, atmosphereFactor);

    // Apply cloud offset and ensure the final alpha is set to the earth alpha
    FragColor = finalColor + vec4(cloudOffset, cloudOffset, cloudOffset, 0.0);
    FragColor.a = earthColor.a;
}
