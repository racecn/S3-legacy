#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform sampler2D earthTexture;
uniform sampler2D earthNormalMap;

void main()
{    
    vec3 normal = texture(earthNormalMap, TexCoords).rgb;
    vec3 color = texture(earthTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
