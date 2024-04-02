#version 330 core
out vec4 FragColor;

uniform vec3 emissiveColor;

void main()
{
    FragColor = vec4(emissiveColor, 1.0);
}
