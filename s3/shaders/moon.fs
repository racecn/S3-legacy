#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D earthTexture;

void main()
{
    vec4 earthColor = texture(earthTexture, TexCoords);
    FragColor = earthColor;
}
