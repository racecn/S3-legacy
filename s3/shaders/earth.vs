#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;
out vec3 ViewDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos; // Add this line

void main()
{
    Normal = aNormal;
    Position = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    vec4 worldPos = model * vec4(aPos, 1.0); // Convert to world space
    ViewDir = normalize(cameraPos - worldPos.xyz); // Calculate view direction
    gl_Position = projection * view * worldPos;
}
